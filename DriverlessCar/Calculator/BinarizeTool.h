#ifndef __SB_BINARIZE_TOOL_H__
#define __SB_BINARIZE_TOOL_H__
#include "IBinarizeTool.h"

namespace sb
{
class BinarizeTool: public IBinarizeTool
{
private:
	double _binarizeThresh;

public:
	BinarizeTool( double binarizeThresh )
		: _binarizeThresh( binarizeThresh ) {}

	int binarize( const cv::Mat& bgrImage, cv::Mat& binImage ) override;
};
}

#endif //!__SB_BINARIZE_TOOL_H__
