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

	CvAdaptiveSkinDetector skinDetector;

	cv::Mat orgImg = cv::imread("testImages\\depth_215.bmp");
	logInfo("origin image channels = %d", orgImg.channels());

	//	*************************************************************************************************
	cv::initModule_nonfree();	//	if use SIFT or SURF
	cv::Ptr<cv::FeatureDetector> features_Detector = cv::FeatureDetector::create("SIFT");
	cv::Ptr<cv::DescriptorExtractor> descriptor_Extrator = cv::DescriptorExtractor::create("SIFT");
	if (features_Detector.empty() || descriptor_Extrator.empty()){
		logInfo("Failed to create detector.");
		cv::destroyAllWindows();
		return -1;
	}

	std::vector<cv::KeyPoint> keyPoints;
	features_Detector->detect(orgImg, keyPoints);
	cv::Mat descriptors;
	descriptor_Extrator->compute(orgImg, keyPoints, descriptors);

	cv::Mat img_Keypoints;
	cv::drawKeypoints(orgImg, keyPoints, img_Keypoints, cv::Scalar::all(-1), 0);
	cv::namedWindow("Image KeyPoints");
	cv::imshow("Image KeyPoints", img_Keypoints);

	//	*************************************************************************************************

	cv::Mat grayImg;
	cv::cvtColor(orgImg, grayImg, CV_RGB2GRAY);
	logInfo("binary image channels = %d, type = %d", grayImg.channels(), grayImg.type());

	cv::Mat invImg(grayImg.rows, grayImg.cols, grayImg.type());
	uchar* grayImg_ptr = grayImg.data;
	uchar* invImg_ptr = invImg.data;
	for (int i = 0; i < grayImg.rows; i++){
		for (int j = 0; j < grayImg.cols; j++){
			invImg_ptr[i*grayImg.cols+j] = grayImg_ptr[i*grayImg.cols+j];
		}
	}
	cv::namedWindow("Inverse image");
	cv::imshow("Inverse image", invImg);


	cv::Mat binImg;
//	cv::adaptiveThreshold(grayImg, binImg, 180, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 21, 15);
	cv::threshold(grayImg, binImg, 230, 250, cv::THRESH_BINARY);

	cv::erode(binImg, binImg, cv::Mat(5, 5, CV_8U), cv::Point(-1, -1), 2);
	cv::dilate(binImg, binImg, cv::Mat(5, 5, CV_8U), cv::Point(-1, -1), 2);

	cv::morphologyEx(binImg, binImg, cv::MORPH_OPEN, cv::Mat(5, 5, CV_8U), cv::Point(-1, -1), 1);
	cv::morphologyEx(binImg, binImg, cv::MORPH_CLOSE, cv::Mat(5, 5, CV_8U), cv::Point(-1, -1), 1);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::Mat tmpBinImg;
	binImg.copyTo(tmpBinImg);
	cv::findContours(tmpBinImg, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	cv::drawContours(orgImg, contours, -1, cv::Scalar(0, 0,255), 2);
	
	cv::imshow(orgStr, orgImg);
	cv::imshow(grayStr, grayImg);
	cv::imshow(binStr, binImg);

	

	if (cv::waitKey(0) == 0x1b)		cv::destroyAllWindows();

	return 0;
}