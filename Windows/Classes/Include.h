#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include <functional>
#include <opencv2/opencv.hpp>

#define CV_2PI 6.2831853
#define CV_E 2.7182818

#define VIDEO_TEST_PATH "../../Debug/sample-3.mp4"
#define PARAMS_PATH "../../Debug/sample-PARAMS.yaml"

#define WINDOW_NAME "Window"

#define KEY_TO_ESCAPE 27

namespace cv
{
typedef cv::Rect_<double> Rect2d;
}



#endif //!__INCLUDE_H__
