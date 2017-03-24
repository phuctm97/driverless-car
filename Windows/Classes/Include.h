#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include <functional>
#include <opencv2/opencv.hpp>

#define CV_2PI 6.2831853
#define CV_E 2.7182818

#define VIDEO_TEST_PAH "../../Debug/video-2.avi"
#define PARAMS_PATH "../../Debug/PARAMS-2.yaml"

#define WINDOW_NAME "Window"

#define KEY_TO_ESCAPE 27

namespace cv
{
typedef cv::Rect_<double> Rect2d;
}



#endif //!__INCLUDE_H__
