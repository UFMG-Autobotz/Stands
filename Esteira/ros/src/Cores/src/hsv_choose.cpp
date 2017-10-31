#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>


#define HUEMAX 179
#define SATMAX 255
#define VALMAX 255

#define MAX_H 179
#define MAX_S 255
#define MAX_V 255

#define max_hue_range 179
#define max_step 3 //nuber of pixel for each hue color


#define window_name "HSV Color Plot"

using namespace std;
using namespace cv;

cv::Mat im_source, HSV;
int H =170;
int S=200;
int V =200;
int R=0;
int G=0;
int B=0;

//Global variable for hsv color wheel plot
int wheel_width=max_hue_range*max_step;
int wheel_hight=50;
int wheel_x=50; //x-position of wheel
int wheel_y=5;//y-position of wheel

//Global variable plot for satuarion-value plot
int S_V_Width=MAX_S;
int S_V_Height=MAX_S;
int S_V_x=10;
int S_V_y=wheel_y+wheel_hight+20;

//Global variable for HSV ploat
int HSV_Width=150;
int HSV_Height=150;
int HSV_x=S_V_x+S_V_Width+30;
int HSV_y=S_V_y+50;

void drawPointers(){
	// Point p(S_V_x+S,S_V_y+(255-V));
	cv::Point p(S,255-V);
	int index=10;
	cv::Point p1,p2;
	p1.x=p.x-index;
	p1.y=p.y;
	p2.x=p.x+index;
	p2.y=p.y;
	cv::Mat roi1(HSV,cv::Rect(S_V_x,S_V_y,S_V_Width,S_V_Height));
	cv::line(roi1, p1, p2,cv::Scalar(255,255,255),1,CV_AA,0);
	p1.x=p.x;
	p1.y=p.y-index;
	p2.x=p.x;
	p2.y=p.y+index;
	cv::line(roi1, p1, p2,cv::Scalar(255,255,255),1,CV_AA,0);
	int x_index=wheel_x+H*max_step;
	if(x_index>=wheel_x+wheel_width)
		x_index=wheel_x+wheel_width-2;
	if(x_index<=wheel_x)
		x_index=wheel_x+2;
	p1.x=x_index;
	p1.y=wheel_y+1;
	p2.x=x_index;
	p2.y=wheel_y+20;
	cv::line(HSV, p1, p2,cv::Scalar(255,255,255),2,CV_AA,0);
	cv::Mat RGB(1,1,CV_8UC3);
	cv::Mat temp;
	RGB=cv::Scalar(H,S,V);
	cv::cvtColor(RGB, temp,CV_HSV2BGR);
	cv::Vec3b rgb=temp.at<cv::Vec3b>(0,0);
	B=rgb.val[0];
	G=rgb.val[1];
	R=rgb.val[2];
	cv::Mat roi2(HSV,cv::Rect(450,130,175,175));
	roi2=cv::Scalar(200,0,200);
	char name[30];
	sprintf(name,"R=%d",R);
	putText(HSV,name, cv::Point(460,155) , FONT_HERSHEY_SIMPLEX, .7, cv::Scalar(5,255,255), 2,8,false );

	sprintf(name,"G=%d",G);
	putText(HSV,name, cv::Point(460,180) , FONT_HERSHEY_SIMPLEX, .7, cv::Scalar(5,255,255), 2,8,false );

	sprintf(name,"B=%d",B);
	putText(HSV,name, cv::Point(460,205) , FONT_HERSHEY_SIMPLEX, .7, cv::Scalar(5,255,255), 2,8,false );


	sprintf(name,"H=%d",H);
	putText(HSV,name, cv::Point(545,155) , FONT_HERSHEY_SIMPLEX, .7, cv::Scalar(5,255,255), 2,8,false );

	sprintf(name,"S=%d",S);
	putText(HSV,name, cv::Point(545,180) , FONT_HERSHEY_SIMPLEX, .7, cv::Scalar(5,255,255), 2,8,false );

	sprintf(name,"V=%d",V);
	putText(HSV,name, cv::Point(545,205) , FONT_HERSHEY_SIMPLEX, .7, cv::Scalar(5,255,255), 2,8,false );
}

void onTrackbar_changed(int, void*){
    //Plot color wheel.
	int hue_range=0;
	int step=1;
	for(int i=wheel_y;i<wheel_hight+wheel_y;i++){
		hue_range=0;
		for(int j=wheel_x;j<wheel_width+wheel_x;j++){
			if(hue_range>=max_hue_range)
				hue_range=0;
			if(step++==max_step){
				hue_range++;
				step=1;
			}
			cv::Vec3b pix;
			pix.val[0]=hue_range;
			pix.val[1]=255;
			pix.val[2]=255;
			HSV.at<cv::Vec3b>(i,j)=pix;
		}
	}
	//Plot for saturation and value
	int sat_range=0;
	int value_range=255;
	for(int i=S_V_y;i<S_V_Height+S_V_y;i++){
		value_range--;
		sat_range=0;
		for(int j=S_V_x;j<S_V_Width+S_V_x;j++){
			cv::Vec3b pix;
			pix.val[0]=H;
			pix.val[1]=sat_range++;
			pix.val[2]=value_range;
			HSV.at<cv::Vec3b>(i,j)=pix;
		}
	}
	//Ploat for HSV
	cv::Mat roi1(HSV,cv::Rect(HSV_x,HSV_y,HSV_Width,HSV_Height));
	roi1=cv::Scalar(H,S,V);
	drawPointers();
	cv::Mat RGB;
	cv::cvtColor(HSV, RGB,CV_HSV2BGR);
	cv::imshow(window_name,RGB);
	cv::imwrite("hsv.jpg",RGB);
}

static void onMouse( int event, int x, int y, int f, void* ){
	if(f&CV_EVENT_FLAG_LBUTTON){
		if( ( (wheel_x<=x) && (x<=wheel_x + wheel_width) ) && ( (wheel_y<=y) && (y<=wheel_y+wheel_hight) ) ){
			H=(x-wheel_x)/ max_step;
			cvSetTrackbarPos("Hue", window_name, H);
		}
		else if( ( (S_V_x<=x) && (x<=S_V_x+S_V_Width) ) && ( (S_V_y<=y) && (y<=S_V_y+S_V_Height) ) ){
			S=x-S_V_x;
			y=y-S_V_y;
			V=255-y;
			cvSetTrackbarPos("Saturation", window_name, S);
			cvSetTrackbarPos("Value", window_name, V);
		}
	}
}

static void onMouse2( int event, int x, int y, int f, void* ){
	if(f&CV_EVENT_FLAG_LBUTTON){
		std::cout << x << " " << y << std::endl;
		cv::Vec3b cor_image = im_source.at<cv::Vec3b>(y,x);
		cv::Mat rgb(1, 1, CV_8UC3, cv::Scalar( cor_image ) ), hsv;
		cv::cvtColor(rgb, hsv,CV_BGR2HSV);
		cv::Vec3b cor_hsv = hsv.at<cv::Vec3b>(0,0);
		H=cor_hsv[0];
		S=cor_hsv[1];
		V=cor_hsv[2];
		std::cout << H << " " << S << " " << V << std::endl;
		cvSetTrackbarPos("Hue", window_name, H);
		cvSetTrackbarPos("Saturation", window_name, S);
		cvSetTrackbarPos("Value", window_name, V);
	}
}

int main(int argc, char **argv){

	HSV.create(390,640,CV_8UC3); //Mat to store clock image
	HSV.setTo(Scalar(200,0,200));
	namedWindow(window_name);
	createTrackbar( "Hue",window_name, &H, HUEMAX, onTrackbar_changed );
	createTrackbar( "Saturation",window_name, &S, SATMAX,onTrackbar_changed );
	createTrackbar( "Value",window_name, &V, VALMAX,onTrackbar_changed);
	onTrackbar_changed(0,0); //initialoze window
	setMouseCallback( window_name, onMouse, 0 );
	std::string mode = argc > 2 ? argv[1] : "";
	bool end_of_process = false;
	if (mode == "video"){
		cv::VideoCapture video;
		video.open(atoi(argv[2]));
		namedWindow("Video");
		setMouseCallback( "Video", onMouse2, 0 );
		while( !end_of_process ){
			video >> im_source;
			if( im_source.empty() ){
				std::cout << "Could not open camera" << std::endl;
				break;
			}
			cv::imshow("Video",im_source);
			char key = waitKey(10);
		    if( 'q' == key )
				end_of_process = true;
		}
	}
	else{
		if (mode == "image"){
			im_source = cv::imread( argv[2]);
			namedWindow(argv[2]);
			cv::imshow(argv[2],im_source);
			setMouseCallback( argv[2], onMouse2, 0 );
		}
		while( !end_of_process ){
			int c;
			char key = waitKey(10);
		    if( 'q' == key )
				end_of_process = true;
		}
	}
	return 0;
}
