//
//  cvtico.cpp
//  icorepo
//
//  Created by kimbomm on 2018. 10. 28...
//  Copyright 2018 kimbomm. All rights reserved.
//
#include<opencv2/opencv.hpp>
#include<windows.h>
#include"cv_ico.h"
int main(int argc,const char* argv[]) {
	if(argc<2){
		std::cerr << "[Usage] cvtico.exe <pngfile>" << std::endl;
		return 1;
	}
	std::string png = argv[1];
	cv::Mat img = cv::imread(png, cv::IMREAD_UNCHANGED);
	if(img.cols>256 && img.cols>=img.rows){
		cv::resize(img,img,cv::Size(256,img.rows*256/img.cols),0,0,cv::INTER_AREA);
	}
	if(img.rows>256 && img.rows>=img.cols){
		cv::resize(img,img,cv::Size(img.cols*256/img.rows,256),0,0,cv::INTER_AREA);
	}
	std::string ico = png.substr(0, png.find_last_of('.')) + ".ico";
	cv::ico::imwrite(ico,img);
	//std::string cur = png.substr(0, png.find_last_of('.')) + ".cur";
	//cv::ico::imwrite(cur,img);

	std::string jpg = png.substr(0, png.find_last_of('.')) + "_readme.png";
	cv::Mat img_jpg = cv::imread(png,cv::IMREAD_UNCHANGED);

	auto letterbox=[](const cv::Mat& img)->cv::Mat{
		int N=std::max(img.cols,img.rows);
		cv::Mat dst=cv::Mat::zeros(N,N,CV_8UC(img.channels())) + cv::Scalar(255,255,255);
		int dx=(N-img.cols)/2;
		int dy=(N-img.rows)/2;
		img.copyTo(dst(cv::Rect(dx,dy,img.cols,img.rows)));
		return dst;
	};
	img_jpg=letterbox(img_jpg);
	cv::resize(img_jpg,img_jpg,cv::Size(64,64),0,0,cv::INTER_AREA);
	cv::imwrite(jpg,img_jpg);
	return 0;
}