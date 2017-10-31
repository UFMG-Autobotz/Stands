#include "color_detection.hpp"
#include <iostream>
#include <algorithm>
#include <stdlib.h>


#define	branco (v >= 206 && g >= 206)
#define	preto (v <= 50 && g <= 50)
#define	vermelho (h < 11 || h >= 171)
#define	laranja (h >= 11 && h < 21)
#define	amarelo (h >= 21 && h < 40)
#define	verde (h >= 40 && h < 81)
#define	ciano (h >= 81 && h < 101)
#define	azul (h >= 101 && h < 121)
#define	roxo (h >= 121 && h < 146)
#define	rosa (h >= 146 && h < 171)

int predict_color(int h, int s, int v, int g, bool bw){
	if (bw){
		if branco
			return 0;
		if preto
			return 1;
	}
	if vermelho
		return 2;
	if laranja
		return 3;
	if amarelo
		return 4;
	if verde
		return 5;
	if ciano
		return 6;
	if azul
		return 7;
	if roxo
		return 8;
	if rosa
		return 9;
	return -1;
}

int get_color_ID(cv::Mat image, bool bw){
	cv::Scalar cor = cv::mean(image);
	cv::Mat rgb(1, 1, CV_8UC3, cor), hsv, gray;
	cv::cvtColor(rgb, hsv,CV_BGR2HSV);
	cv::cvtColor(rgb, gray,CV_BGR2GRAY);
	cv::Vec3b cor_hsv = hsv.at<cv::Vec3b>(0,0);
	int hue = cor_hsv[0];
	int sat = cor_hsv[1];
	int val = cor_hsv[2];
	int cor_gray = gray.at<uchar>(0,0);
	
//	std::cout << hsv << " - " << gray <<  " - ";
//	cv::Mat show(500, 500, CV_8UC3, cor);
//	cv::imshow( "Mean_color", show );
//	cv::waitKey(50);

	return predict_color(hue, sat, val, cor_gray, bw);
}

float* get_color_percent(cv::Mat image, bool bw){
	cv::Mat img = image.clone();
//	CV_Assert(img.depth() == CV_8U);
    const int channels = img.channels();
	float *cores = (float*)calloc(10,sizeof(float));
    switch(channels)
    {
    case 1:
        {
            std::cout << "Gray_scale image" << std::endl;
			return NULL;
            break;
        }
    case 3:
        {
            cv::MatIterator_<cv::Vec3b> it, end;
			cv::Mat gray, hsv;
            for( it = img.begin<cv::Vec3b>(), end = img.end<cv::Vec3b>(); it != end; ++it)
            {
				cv::Scalar cor((*it));
				cv::Mat rgb(1, 1, CV_8UC3, cor);
				cv::cvtColor(rgb, hsv,CV_BGR2HSV);
				cv::cvtColor(rgb, gray,CV_BGR2GRAY);
				cv::Vec3b cor_hsv = hsv.at<cv::Vec3b>(0,0);

				int hue = cor_hsv[0];
				int sat = cor_hsv[1];
				int val = cor_hsv[2];
				int cor_gray = gray.at<uchar>(0,0);

				cores[predict_color(hue, sat, val, cor_gray, bw)]+=1;
            }
        }
    }
	float tot=0;
	for (int i = 0; i < 10; i ++)
	{
		tot+=cores[i];
	}

	for (int i = 0; i < 10; i ++)
	{
		std::cout << cores[i] << "-";
		cores[i]/=tot;
		std::cout << cores[i] << " ";
	}
	std::cout << std::endl;
    return cores;
}

cv::Mat get_color_per_pixel_ID(cv::Mat image, bool bw){
	cv::Mat img = image.clone();
//	CV_Assert(img.depth() == CV_8U);
	cv::Mat color_mat = cv::Mat::zeros( img.size(), CV_8UC1 );
    const int channels = img.channels();
//	unsigned char **cores = (unsigned char**)calloc(img.rows,sizeof(unsigned char*));
//	for (int i = 0; i < img.rows; i ++)
//		cores[i] = (unsigned char*)calloc(img.cols,sizeof(unsigned char));
    switch(channels)
    {
    case 1:
        {
            std::cout << "Gray_scale image" << std::endl;
			return cv::Mat();
        }
    case 3:
        {
			cv::MatIterator_<cv::Vec3b> it, end;
			cv::MatIterator_<uchar> it2, end2;
			cv::Mat gray, hsv;
			int i=0;
			for( it = img.begin<cv::Vec3b>(), it2 = color_mat.begin<uchar>(), end = img.end<cv::Vec3b>(); it != end; ++it, ++it2){
				int j = i%img.cols, k = i/img.cols;
				cv::Scalar cor((*it));
				cv::Mat rgb(1, 1, CV_8UC3, cor);
				cv::cvtColor(rgb, hsv,CV_BGR2HSV);
				cv::cvtColor(rgb, gray,CV_BGR2GRAY);
				cv::Vec3b cor_hsv = hsv.at<cv::Vec3b>(0,0);

				int hue = cor_hsv[0];
				int sat = cor_hsv[1];
				int val = cor_hsv[2];
				int cor_gray = gray.at<uchar>(0,0);
				*it2 = predict_color(hue, sat, val, cor_gray, bw)*25;

//				cores[j][k] = predict_color(hue, sat, val, cor_gray, bw)*25;
            }
        }
    }

	cv::imshow( "Mean_color", color_mat );
	cv::waitKey(50);
return color_mat;
//	std::cout << std::endl;
//    return cores;
}

bool is_in_range(cv::Mat image, int h, int s, int v, int h_delta, int s_delta, int v_delta){
	cv::Scalar cor = cv::mean(image);
	cv::Mat rgb(1, 1, CV_8UC3, cor), hsv;
	cv::cvtColor(rgb, hsv,CV_BGR2HSV);
	cv::Vec3b cor_hsv = hsv.at<cv::Vec3b>(0,0);
	int hue = cor_hsv[0];
	int sat = cor_hsv[1];
	int val = cor_hsv[2];
//	std::cout << hue << " " << sat << " " << val << " ";
	return (abs(h - hue) <= h_delta || abs(h - hue) >= 180 - h_delta ) && abs(s - sat) <= s_delta && abs(v - val) <= v_delta;
}

cv::Mat in_range_mask(cv::Mat image, int h, int s, int v, int h_delta, int s_delta, int v_delta){
	cv::Mat hsv, return_im;
	cv::cvtColor(image, hsv,CV_BGR2HSV);
	cv::inRange(hsv, cv::Scalar(std::max(0,h-h_delta), std::max(0,s-s_delta), std::max(0,v-v_delta)), cv::Scalar(std::min(180,h+h_delta), std::min(255,s+s_delta), std::min(255,v+v_delta)), return_im);
	if (h-h_delta<0 || h+h_delta>179){
		cv::Mat acum = cv::Mat::zeros(return_im.rows, return_im.cols,CV_32FC1);
		cv::accumulate(return_im, acum);
		if(h-h_delta<0){
			cv::inRange(hsv, cv::Scalar( 180 - abs(h-h_delta), std::max(0,s-s_delta), std::max(0,v-v_delta)), cv::Scalar(180, std::min(255,s+s_delta), std::min(255,v+v_delta)), return_im);
		}
		else{
			cv::inRange(hsv, cv::Scalar( 0, std::max(0,s-s_delta), std::max(0,v-v_delta)), cv::Scalar( abs(h+h_delta-180), std::min(255,s+s_delta), std::min(255,v+v_delta)), return_im);
		}
		cv::accumulate(return_im, acum);
		acum = acum /2;
		acum.convertTo(return_im,CV_8U);
		threshold( return_im, return_im, 100, 255, CV_THRESH_BINARY );
	}
	cv::Mat element = getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5), cv::Point(2, 2) );
	cv::erode(return_im,return_im,element);
	cv::erode(return_im,return_im,element);
	return return_im;
}
