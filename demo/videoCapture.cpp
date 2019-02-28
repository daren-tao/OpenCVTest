/*

 */
#include <cstdio>
#include <cstdlib>

#include "Logger.hpp"

#include "opencv2/opencv.hpp"

const std::string wndName = "video capture";

int main(int argc, char** argv)
{
    cv::namedWindow(wndName);
    cv::moveWindow(wndName, 0, 0);

    cv::VideoCapture camera;
    if(!camera.open(0)) return -1;
    
    cv::Mat frame;
    while(camera.read(frame))
    {
        cv::flip(frame, frame, 1);
        
        cv::imshow(wndName, frame);
        
        int key = cv::waitKey(10);
        if(key == 0x1b) break;
    }
    
	return 0;
}
