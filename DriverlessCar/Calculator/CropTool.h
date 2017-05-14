#ifndef __SB_CROP_TOOL_H__
#define __SB_CROP_TOOL_H__
#include "ICropTool.h"

namespace sb
{
class CropTool : public ICropTool
{
private:
	cv::Rect _cropBox;

public:
	CropTool( const cv::Rect& cropBox )
		: _cropBox( cropBox ) {}

	int crop( const cv::Mat& inputImage, cv::Mat& outputImage ) override;
};
}

#endif //!__SB_CROP_TOOL_H__