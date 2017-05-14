#ifndef __SB_FLIP_TOOL_H__
#define __SB_FLIP_TOOL_H__

#include "IFlipTool.h"

namespace sb
{
class FlipTool : public IFlipTool
{
public:
	int flip( const cv::Mat& inputImage, cv::Mat& outputImage ) override;
};
}

#endif //!__SB_FLIP_TOOL_H__
