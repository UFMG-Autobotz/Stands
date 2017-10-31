#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "../lib/color_detection.hpp"

#include <ros/ros.h>
#include "std_msgs/Float32.h"
#include <geometry_msgs/Point32.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Polygon.h>



#define POINT_R 2

#define BRANCO 0
#define PRETO 1
#define VERMELHO 2
#define LARANJA 3
#define AMARELO 4
#define VERDE 5
#define CIANO 6
#define AZUL 7
#define ROXO 8
#define ROSA 9

void pubCoords(ros::Publisher Coords_pub, std::vector<cv::Point2f> center, cv::Mat& image){
	geometry_msgs::Polygon Coords_msg;
	cv::RNG rng(12345);
	for( int l = 0; l< center.size(); l++ ){
			cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
			// cv::drawContours( drawing, contours_poly, l, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
			cv::circle( image, center[l], 1, color, -1, 8, 0 );
			//for each location, push a corresponding centroid
			geometry_msgs::Point32 P;
			P.x = center[l].x;
			P.y = center[l].y;
			Coords_msg.points.push_back(P);
	}
	Coords_pub.publish(Coords_msg);
}

void pubTarget(ros::Publisher Target_pub, std::vector<cv::Point2f> center, cv::Mat& image, float X_len, float Y_len, int timeout_value){
	static float last_loc = 200;
	static time_t start=0, end;
	geometry_msgs::Point Target_msg;
	int maior = 0, ind = -1;
	for (int l = 0; l < center.size(); l++)
		if(center[l].y > maior){
			maior = center[l].y;
			ind = l;
		}
	if(ind != -1){
		float x = (float) center[ind].x, y = (float) center[ind].y;
		cv::circle( image, center[ind], 1, cv::Scalar( 0, 0, 255), -1, 8, 0 );
		Target_msg.x = last_loc = (x-(X_len/2))*200/X_len;
		Target_msg.y = (Y_len - y)*100/Y_len;
		last_loc = (last_loc>0) ? 200:-200;
		time(&start);
	}
	else {
		time(&end);
		std::cout << end-start << std::endl;
		if(end-start < timeout_value){
			Target_msg.x = last_loc;
			Target_msg.y = 101;
		}
		else{
			Target_msg.x = last_loc;
			Target_msg.y = -1;
		}
	}
	
	Target_pub.publish(Target_msg);
}

int main (int argc, char ** argv){
	//init ROS
	ros::init(argc, argv, "discreto_ROS");

	ros::NodeHandle nh;
	ros::Publisher Coords_pub = nh.advertise<geometry_msgs::Polygon>("/visao/cores", 1);
	ros::Publisher Target_pub = nh.advertise<geometry_msgs::Point>("esteira/visao", 1);
	ros::Publisher screenX_pub = nh.advertise<std_msgs::Float32>("/visao/screenX", 1);
	ros::Publisher screenY_pub = nh.advertise<std_msgs::Float32>("/visao/screenY", 1);
	if(!Target_pub || !Coords_pub || !screenX_pub || !screenY_pub){
		ROS_ERROR("discreto_ROS: One of the publishers could not be initialized!");
	}

	// Parâmetros
	int NUM_L, NUM_C, NUM_FRAMES, _H, _S, _V, _EH, _ES, _EV, timeout_value, camera_ID;
	nh.getParam("visao/_H",_H);
	nh.getParam("visao/_S",_S);
	nh.getParam("visao/_V",_V);
	nh.getParam("visao/_EH",_EH);
	nh.getParam("visao/_ES",_ES);
	nh.getParam("visao/_EV",_EV);
	nh.getParam("visao/NUM_L",NUM_L);
	nh.getParam("visao/NUM_C",NUM_C);
	nh.getParam("visao/NUM_FRAMES",NUM_FRAMES);
	nh.getParam("visao/timeout_value",timeout_value);
	nh.getParam("visao/camera_ID",camera_ID);

	//create a Rate object to control the execution rate
	//of our main loop
	ros::Rate loop_rate(10);

	cv::VideoCapture video;
	video.open(camera_ID);

	cv::Mat src, roi, points_ID(NUM_L, NUM_C, CV_8UC3), show, point_gray, show_gray;
	bool end_of_process = false;
	cv::Mat im_vec[NUM_FRAMES];
	for(int l = 0; l < NUM_FRAMES ; l++)
	im_vec[l] = cv::Mat::zeros(NUM_L, NUM_C,CV_8U);
	while( !end_of_process ){
		video >> src;
		if( src.empty() ){
			std::cout << "Could not open camera" << std::endl;
			break;
		}
		for (int j = 0, lin = 0; j + src.rows/NUM_L <= src.rows; j+=src.rows/NUM_L, lin++){
			for (int k = 0, col=0; k + src.cols/NUM_C <= src.cols; k+=src.cols/NUM_C, col++){
				cv::Rect piece = cv::Rect(k, j, src.cols/NUM_C, src.rows/NUM_L);
				roi = src(piece);
				cv::Scalar point_color;
				// int color_ID = get_color_ID(roi, true);
				// if (color_ID == AMARELO)
				if(is_in_range(roi, _H, _S, _V, _EH, _ES, _EV))
					point_color = cv::Scalar(255,255,255);
				else
					point_color = cv::Scalar(0,0,0);
				cv::circle( src, cv::Point(k + src.cols/(2*NUM_C), j + src.rows/(2*NUM_L)), POINT_R, point_color, -1, 8 );
				points_ID(cv::Range(lin, lin + 1), cv::Range(col, col + 1)) = point_color;
			}
		}

		for(int l = NUM_FRAMES-1; l > 0 ; l--)
			im_vec[l] = im_vec[l-1].clone();
		cv::cvtColor( points_ID, im_vec[0], CV_BGR2GRAY );

		point_gray   = cv::Mat::zeros(points_ID.rows, points_ID.cols,CV_32F);
		for(int l = 0; l < NUM_FRAMES ; l++)
			cv::accumulate(im_vec[l], point_gray);
		point_gray = point_gray /NUM_FRAMES;
		point_gray.convertTo(point_gray,CV_8U);

		threshold( point_gray, point_gray, 254, 255, CV_THRESH_BINARY );
		show_gray = point_gray.clone();

		std::vector<std::vector<cv::Point> > contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours( point_gray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

		std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
		std::vector<cv::Point2f>center( contours.size() );
		std::vector<float>radius( contours.size() );

		for( int l = 0; l < contours.size(); l++ ){
			cv::approxPolyDP( cv::Mat(contours[l]), contours_poly[l], 3, true );
			cv::minEnclosingCircle( (cv::Mat)contours_poly[l], center[l], radius[l] );
		}

		cv::Mat drawing = cv::Mat::zeros( point_gray.size(), CV_8UC3 );

		//Caso algo tenha sido detectado, desenha na imagem e publica as coordenadas
		// pubCoords(Coords_pub, center, drawing);
		pubTarget(Target_pub, center, drawing, drawing.cols, drawing.rows, timeout_value);
		std_msgs::Float32 x, y;
		x.data = src.cols;
		y.data = src.rows;
		screenX_pub.publish(x);
		screenY_pub.publish(y);

		cv::resize(drawing, drawing, cv::Size(src.cols, src.rows));
		cv::imshow( "Contours", drawing );

		cv::resize(points_ID, show, cv::Size(src.cols, src.rows));
		cv::imshow( "Image_color", show );

		cv::resize(show_gray, show_gray, cv::Size(src.cols, src.rows));
		cv::imshow( "Image_gray", show_gray );

		cv::imshow( "Image", src );
		// cv::waitKey(0);
		char key = cv::waitKey(10);
		if( 'q' == key )
			end_of_process = true;

		//spin ROS
		ros::spinOnce();

		//wait
		loop_rate.sleep();

	}
	return 0;
}