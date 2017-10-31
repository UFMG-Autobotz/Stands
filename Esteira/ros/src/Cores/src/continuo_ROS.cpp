#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "../lib/color_detection.hpp"

#include <ros/ros.h>
#include "std_msgs/Float32.h"
#include <geometry_msgs/Point32.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Polygon.h>

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

#define POINT_R 2

// Flags para o código de movimento
#define BOLA_PRESENTE -1
#define BOLA_AUSENTE 0

void pubCoords(ros::Publisher Coords_pub, std::vector<cv::Point2f> center, cv::Mat& image){
	geometry_msgs::Polygon Coords_msg;
	cv::RNG rng(12345);
	for( int l = 0; l< center.size(); l++ ){
			cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
			// cv::drawContours( drawing, contours_poly, l, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
			cv::circle( image, center[l], 15, color, -1, 8, 0 );
			//for each location, push a corresponding centroid
			geometry_msgs::Point32 P;
			P.x = center[l].x;
			P.y = center[l].y;
			Coords_msg.points.push_back(P);
	}
	Coords_pub.publish(Coords_msg);
}

geometry_msgs::Point Target_msg;

void pubTarget(ros::Publisher Target_pub, std::vector<cv::Point2f> center, cv::Mat& image, float X_len, float Y_len, int timeout_value){
	static float last_loc = 200;
	static time_t start=0, end;
	
	int maior = 0, ind = -1;
	for (int l = 0; l < center.size(); l++)
		if(center[l].y > maior){
			maior = center[l].y;
			ind = l;
		}
	if(ind != -1){
		float x = (float) center[ind].x, y = (float) center[ind].y;
		cv::circle( image, center[ind], 15, cv::Scalar( 0, 0, 255), -1, 8, 0 );
		Target_msg.x = last_loc = (x-(X_len/2))*200/X_len;
		Target_msg.y = (Y_len - y)*100/Y_len;
		Target_msg.z = BOLA_PRESENTE;						// Avisa o código de movimento que a bola está na tela
		last_loc = (last_loc>0) ? 200:-200;
		time(&start);
	}
	else {
		time(&end);
		std::cout << end-start << std::endl;

		if(end-start < timeout_value)
			Target_msg.z = 1 - (float)(end-start)/(float)timeout_value;	// Envia o temporizador normalizado ao código de movimento
		else
			Target_msg.z = BOLA_AUSENTE;					// Avisa o código de movimento que a bola foi perdida
	}
	
	Target_pub.publish(Target_msg);
}

int main (int argc, char ** argv){
	//init ROS
	ros::init(argc, argv, "continuo_ROS");
	
	ros::NodeHandle nh;
	ros::Publisher Coords_pub = nh.advertise<geometry_msgs::Polygon>("/visao/cores", 1);
	ros::Publisher Target_pub = nh.advertise<geometry_msgs::Point>("esteira/visao", 1);
	ros::Publisher screenX_pub = nh.advertise<std_msgs::Float32>("/visao/screenX", 1);
	ros::Publisher screenY_pub = nh.advertise<std_msgs::Float32>("/visao/screenY", 1);
	if(!Target_pub || !Coords_pub || !screenX_pub || !screenY_pub){
		ROS_ERROR("continuo_ROS: One of the publishers could not be initialized!");
	}

	// Parâmetros
	int NUM_FRAMES, _H, _S, _V, _EH, _ES, _EV, camera_ID;
	float porc_detect, timeout_value;
	nh.getParam("visao/HSV/H",_H);
	nh.getParam("visao/HSV/S",_S);
	nh.getParam("visao/HSV/V",_V);
	nh.getParam("visao/HSV/EH",_EH);
	nh.getParam("visao/HSV/ES",_ES);
	nh.getParam("visao/HSV/EV",_EV);
	nh.getParam("visao/NUM_FRAMES",NUM_FRAMES);
	nh.getParam("visao/porc_detect",porc_detect);
	nh.getParam("visao/camera_ID",camera_ID);
	nh.getParam("esteira/timeout_value",timeout_value);

	bool calibragem = false;
	
	//create a Rate object to control the execution rate
	//of our main loop
	ros::Rate loop_rate(10);

	cv::VideoCapture video;
	video.open(camera_ID);
	cv::Mat src, points_ID, show, point_gray, show_gray;
	bool end_of_process = false;
	cv::Mat im_vec[NUM_FRAMES];
	video >> src;

	if( src.empty() ){
		std::cout << "Could not open camera" << std::endl;
		return 0;
	}

	for(int l = 0; l < NUM_FRAMES ; l++)
		im_vec[l] = cv::Mat::zeros(src.rows, src.cols, CV_8U);

	while( !end_of_process ){
		video >> src;
		if( src.empty() ){
			std::cout << "Could not open camera" << std::endl;
			break;
		}
		points_ID = in_range_mask(src, _H, _S, _V, _EH, _ES, _EV);

		for(int l = NUM_FRAMES-1; l > 0 ; l--)
			im_vec[l] = im_vec[l-1].clone();
		im_vec[0] = points_ID.clone();

		point_gray   = cv::Mat::zeros(points_ID.rows, points_ID.cols,CV_32F);
		for(int l = 0; l < NUM_FRAMES ; l++)
			cv::accumulate(im_vec[l], point_gray);
		point_gray = point_gray /NUM_FRAMES;
		point_gray.convertTo(point_gray,CV_8U);

		threshold( point_gray, point_gray, 254*((int)(porc_detect/100.0)), 255, CV_THRESH_BINARY );
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
		pubCoords(Coords_pub, center, drawing);
		pubTarget(Target_pub, center, drawing, src.cols, src.rows, timeout_value);
		std_msgs::Float32 x, y;
		x.data = src.cols;
		y.data = src.rows;
		screenX_pub.publish(x);
		screenY_pub.publish(y);

		//cv::resize(drawing, drawing, cv::Size(src.cols, src.rows));
		cv::imshow( "Contours", drawing );

		//cv::resize(points_ID, show, cv::Size(src.cols, src.rows));
		cv::imshow( "Image_color", points_ID );

		//cv::resize(show_gray, show_gray, cv::Size(src.cols, src.rows));
		cv::imshow( "Image_gray", show_gray );

		char key = cv::waitKey(10);

		if(key == 'q')
			end_of_process = true;
		else if(key == 'c')
			calibragem = !calibragem;

		if(calibragem)
		{
			cv::Point2f aux;

			aux.x = src.cols/2;
			aux.y = src.rows/2;
			cv::circle(src, aux, 15, cv::Scalar(255,255,0), 2, 8, 0 );
			
			aux.x = src.cols;
			cv::circle(src, aux, 15, cv::Scalar(255,255,0), 2, 8, 0 );

			aux.x = src.cols/2;
			aux.y = 0;
			cv::circle(src, aux, 15, cv::Scalar(255,255,0), 2, 8, 0 );
		}

		cv::imshow( "Image", src );
		// cv::waitKey(0);

		//spin ROS
		ros::spinOnce();
		
		//wait
		loop_rate.sleep();
	}
	return 0;
}