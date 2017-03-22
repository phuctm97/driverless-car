#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include <functional>
#include <opencv2/opencv.hpp>

#define COLOR_FRAME_WIDTH 640
#define COLOR_FRAME_HEIGHT 480
#define COLOR_FRAME_RATE 33

#define DEPTH_FRAME_WIDTH 640
#define DEPTH_FRAME_HEIGHT 480
#define DEPTH_FRAME_RATE 33

#define CV_2PI 6.2831853
#define CV_E 2.7182818

#define WINDOW_NAME "Window"
#define KEY_TO_ESCAPE 27

namespace cv
{
typedef cv::Rect_<double> Rect2d;
}

#endif //!__INCLUDE_H__
