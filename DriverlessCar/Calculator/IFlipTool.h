#ifndef __SB_I_FLIP_TOOL_H__
#define __SB_I_FLIP_TOOL_H__

#include <opencv2/opencv.hpp>

namespace sb
{
class IFlipTool
{
public:
	virtual ~IFlipTool() = default;

	virtual int flip( const cv::Mat& inputImage, cv::Mat& outputImage ) = 0;
};
}

#endif //!__SB_I_FLIP_TOOL_H__
