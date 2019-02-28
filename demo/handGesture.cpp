/***************************************************************
 *
 ***************************************************************/
#include <iostream>
#include <stdarg.h>
#include <conio.h>
#include <sstream>

using namespace cv;


#ifdef _DEBUG_LOG_MSG_
	#define logInfo( format, ... )	printf( "%s\t[%d]\t"format"\n", __FILE__, __LINE__, ##__VA_ARGS__ );
#else
	#define logInfo( format, ... )
#endif


int key;

#define DISPLAY_ROI true
#define DISPLAY_CONTOUR true
#define DISPLAY_ROI_TIGHT_BOUND true
#define DISPLAY_FINGER true
#define DISPLAY_FINGER_NUMBER true




#define PI 3.1415962

struct myclass {
	bool operator() (cv::Point pt1, cv::Point pt2) { return (pt1.x < pt2.x); }
} xCompare;

struct myclass2 {
	bool operator() (cv::Point pt1, cv::Point pt2) { return (pt1.x > pt2.x); }
} xCompareReverse;


int main(int argc, char** argv)
{
	cv::Mat orgImg = cv::imread("testImages\\depth_148.bmp");
	logInfo("origin image wide = %d, high = %d, channels = %d", orgImg.cols, orgImg.rows, orgImg.channels());

	cv::Mat grayImg;
	cv::cvtColor(orgImg, grayImg, CV_RGB2GRAY);
	logInfo("gray image channels = %d, type = %d", grayImg.channels(), grayImg.type());
	cv::imshow("Gray Image", grayImg);

	cv::GaussianBlur(grayImg.clone(), grayImg, cv::Size(1,1), 0);

	cv::Mat binImg;
	cv::threshold(grayImg, binImg, 200, 250, cv::THRESH_BINARY);

	cv::erode(binImg, binImg, Mat(), Point(-1, -1), 3);	//	EROE!  
	cv::dilate(binImg, binImg, Mat(), Point(-1, -1), 2);		//	DILATE!

	cv::imshow("Binary Image", binImg);

	vector<vector<Point>>contours;
	vector<vector<Point>>contoursFiltered;
	vector<Vec4i> hierarchy;
	cv::findContours(binImg, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

	double maxSizeSoFar = -1;
	int minSizeLimit = 5000;
	int maxSizeLimit = 80000;
	for (int i = 0; i < contours.size(); i++)  // check all contours saved in contour vector,
	{
		if (hierarchy[i][3] > 0)
			continue;
		double contourSize = contourArea(contours[i], false);
		if (contourSize <= minSizeLimit)
			continue;
		if (contourSize >= maxSizeLimit)
			continue;
		// from there, only keep the longest contour in contoursFiltered.
		if ((contourSize >= maxSizeSoFar))
		{
			if (contoursFiltered.size() != 0)
				contoursFiltered.pop_back();
			contoursFiltered.push_back(contours.at(i));
		}
	}
	int numOfContours = contoursFiltered.size();
	vector<Moments> mu(contoursFiltered.size());
	vector<Point2f> mc(contoursFiltered.size());
	CvSeq* result;   //hold sequence of points of a contour
	CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours
	int roi_x_left = orgImg.cols;
	int roi_x_right = 0;
	int roi_y_top = orgImg.rows;
	int roi_y_bot = 0;
	int roi_x_center = -1;
	int roi_y_center = -1;
	for (int i = 0; i<numOfContours; i++) {
		rectangle(orgImg, boundingRect(contoursFiltered[i]), Scalar(0, 0, 255), 1, 8, 0);

		int roi_width = boundingRect(contoursFiltered[0]).width;
		int roi_height = boundingRect(contoursFiltered[0]).height;

		roi_x_left = boundingRect(contoursFiltered[i]).x;
		roi_x_right = roi_x_left + roi_width;
		roi_y_top = boundingRect(contoursFiltered[i]).y;
		roi_y_bot = roi_y_top + roi_height;
		roi_x_center = roi_x_left + roi_width / 2;;
		roi_y_center = roi_y_top + roi_height / 2;

		if (DISPLAY_ROI)
		{
			circle(orgImg, Point(roi_x_left, roi_y_top),		5, Scalar(121, 77, 160), -2);
			circle(orgImg, Point(roi_x_right, roi_y_top),		5, Scalar(121, 77, 160), -2);
			circle(orgImg, Point(roi_x_left, roi_y_bot),		5, Scalar(121, 77, 160), -2);
			circle(orgImg, Point(roi_x_right, roi_y_bot),		5, Scalar(121, 77, 160), -2);
			circle(orgImg, Point(roi_x_center, roi_y_center),	3, Scalar(121, 77, 160), 2);
		}
		mu[i] = moments(contoursFiltered[i], false);
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
		circle(orgImg, mc[i], 5, Scalar(150, 20, 191), -1, 8, 0);
		for (int j = 0; j < contoursFiltered[i].size(); j++)
		{
			if (contoursFiltered[i].at(j).y >((mc[i].y + roi_y_bot) / 2))
			{
				contoursFiltered[i].at(j).y = ((mc[i].y + roi_y_bot) / 2);
				contoursFiltered[i].at(j).x = contoursFiltered[i].at(j - 1).x;
			}
		}
		if (DISPLAY_CONTOUR)
			cv::drawContours(orgImg, contoursFiltered, i, cv::Scalar(0, 0, 255), 2);   // last -1: fill inside

		vector<RotatedRect> minRect(numOfContours);
		for (int i = 0; i < contoursFiltered.size(); i++)
			minRect[i] = minAreaRect(Mat(contoursFiltered[i]));

		Point2f rect_points[4];
		minRect[i].points(rect_points);
		for (int j = 0; j < 4; j++){
			if (DISPLAY_ROI_TIGHT_BOUND)
				line(orgImg, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 255, 255), 1, 8);
		}
	}
	std::vector<cv::Point> approx;
	for (int i = 0; i < contoursFiltered.size(); i++)
	{
		cv::approxPolyDP(
			cv::Mat(contoursFiltered[i]),
			approx,
			cv::arcLength(cv::Mat(contoursFiltered[i]), true) * 0.02,
			true
			);
	}
	for (int i = 0; i < approx.size(); i++)
	{
		if (approx.at(i).y > mc[0].y)
		{
			std::swap(approx.at(i), approx.back());
			approx.pop_back();
		}
	}

	vector <double> fingerLength;
	double averageFingerLength = 0;
	for (int i = 0; i < approx.size(); i++)
	{
		cv::Point a(approx.at(i).x, approx.at(i).y);
		cv::Point b(mc[0].x, mc[0].y);
		double res = cv::norm(a - b);//Euclidian distance
		fingerLength.push_back(res);
		averageFingerLength += res;
	}
	averageFingerLength = averageFingerLength / fingerLength.size();
	if (approx.size() != 0)
		circle(orgImg, Point(mc[0].x, mc[0].y), averageFingerLength, Scalar(0, 0, 0), 1);

	double dist_FristWeb = 0;
	double dist_LastWeb = 0;
	bool thumbRight = true;
	if (approx.size() > 1){
		cv::Point FW_Top(approx.at(1).x, approx.at(1).y);
		cv::Point LW_Top(approx.at(approx.size() - 2).x, approx.at(approx.size() - 2).y);
		cv::Point b(mc[0].x, mc[0].y);
		dist_FristWeb = cv::norm(FW_Top - b);//Euclidian distance
		dist_LastWeb = cv::norm(LW_Top - b);

		if (dist_FristWeb <= dist_LastWeb) thumbRight = false;
	}
	if (approx.size() > 1){
		for (int i = 0; i < approx.size() - 1; i++)
		{
			if (fingerLength.at(i) < (1.2)*averageFingerLength)
			{
				fingerLength.erase(fingerLength.begin() + i);
				approx.erase(approx.begin() + i);
			}
		}
	}

	if (!thumbRight)   // thumb on left
		std::sort(approx.begin(), approx.end(), xCompare);
	else  // thumb on right
		std::sort(approx.begin(), approx.end(), xCompareReverse);
	if (approx.size() != 0)
	{
		if ((approx.size() > 1))
		{
			float x1 = float(mc[0].x);
			float y1 = float(mc[0].y);
			int middleIndex = approx.size() / 2;
			float x2 = float(approx[middleIndex].x);
			float y2 = float(approx[middleIndex].y);

			int angle = int(atan((y1 - y2) / (x2 - x1)) * 180 / PI);
			std::stringstream ss1;
			string angleString = ss1.str();
			putText(orgImg, angleString.c_str(), Point(int(x1), (int(y2) + int(y1)) / 2),
				FONT_HERSHEY_SIMPLEX, 5, cv::Scalar(0, 0, 0), 3);
		}
	}

	if (DISPLAY_FINGER)
	for (int i = 0; i < approx.size(); i++)
	{
		circle(orgImg, approx.at(i), 5, Scalar(232, 162, 0), -1);
		std::stringstream ss;
		ss << i;
		String index = ss.str();

		if (DISPLAY_FINGER_NUMBER)
			putText(orgImg, index.c_str(), approx.at(i),
			FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 3);

	}
	contoursFiltered.clear();
	contours.clear();


	cv::imshow("Origin Image", orgImg);
	
	if (cv::waitKey(0) == 0x1b)		cv::destroyAllWindows();

	return 0;
}