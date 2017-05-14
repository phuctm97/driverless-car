#ifndef __SB_I_BINARIZE_TOOL_H__
#define __SB_I_BINARIZE_TOOL_H__

#include <opencv2/opencv.hpp>

namespace sb
{
class IBinarizeTool
{
public:
	virtual ~IBinarizeTool() = default;

	virtual int binarize( const cv::Mat& bgrImage, cv::Mat& binImage ) = 0;
};
}

#endif //!__SB_I_BINARIZE_TOOL_H__
