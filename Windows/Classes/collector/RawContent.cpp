#include "RawContent.h"

void sb::create( sb::RawContent* rawContent, sb::Params* params )
{
	rawContent->colorImage = cv::Mat::zeros( params->COLOR_FRAME_SIZE, CV_8UC3 );
}

<<<<<<< HEAD
void sb::release( sb::RawContent* rawContent )
{
	rawContent->colorImage.release();
}
=======
const cv::Mat& sb::RawContent::getColorImage() const { return _colorImage; }

void sb::RawContent::setColorImage( const cv::Mat& colorImage ) { _colorImage = colorImage; }

>>>>>>> master
