#include "BinarizeTool.h"

int sb::BinarizeTool::binarize( const cv::Mat& bgrImage, cv::Mat& binImage )
{
	cv::cvtColor( bgrImage, binImage, cv::COLOR_BGR2GRAY );
	cv::threshold( binImage, binImage, _binarizeThresh, 255, cv::THRESH_BINARY );
	return 0;
}
