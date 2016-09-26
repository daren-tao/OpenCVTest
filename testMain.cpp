#include <iostream>
#include <stdarg.h>
#include <conio.h>

#include "algoFunc.h"


#ifndef _DEBUG_NO_LOG_MSG_
	#define logInfo( format, ... )	printf( "%s\t[%d]\t"format"\n", __FILE__, __LINE__, ##__VA_ARGS__ );
#else
	#define logInfo( format, ... )
#endif


std::string orgStr = "Origin Image";
std::string grayStr = "Gray Image";
std::string binStr = "Binary Image";

int main(int argc, char** argv)
{
	cv::namedWindow(orgStr);
	cv::namedWindow(grayStr);
	cv::namedWindow(binStr);

	cv::Mat orgImg = cv::imread("testImages\\depth_215.bmp");
	logInfo("origin image channels = %d", orgImg.channels());
	cv::imshow(orgStr, orgImg);

	cv::Mat grayImg;
	cv::cvtColor(orgImg, grayImg, CV_RGB2GRAY);
	logInfo("binary image channels = %d", grayImg.channels());
	cv::imshow(grayStr, grayImg);

	cv::Mat binImg;
//	cv::adaptiveThreshold(grayImg, binImg, 180, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 21, 15);
	cv::threshold(grayImg, binImg, 230, 250, cv::THRESH_BINARY);
	cv::imshow(binStr, binImg);


	if (cv::waitKey(0) == 0x1b)
		cv::destroyAllWindows();

	return 0;
}