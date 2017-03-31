#include "RawContent.h"

void sb::RawContent::create( const sb::Params & params )
{
	_colorImage = cv::Mat::zeros( params.COLOR_FRAME_SIZE, CV_8UC3 );
}

const cv::Mat& sb::RawContent::getColorImage() const { return _colorImage; }

void sb::RawContent::setColorImage( const cv::Mat& colorImage ) { _colorImage = colorImage; }

