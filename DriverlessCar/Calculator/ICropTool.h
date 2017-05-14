#ifndef __SB_I_CROP_TOOL_H__
#define __SB_I_CROP_TOOL_H__

#include <opencv2/opencv.hpp>

namespace sb
{
class ICropTool
{
public:
	virtual ~ICropTool() = default;

	virtual int crop( const cv::Mat& inputImage, cv::Mat& outputImage ) = 0;
};
}

#endif //!__SB_I_CROP_TOOL_H__
