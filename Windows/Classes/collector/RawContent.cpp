#include "RawContent.h"

void sb::create( sb::RawContent* rawContent, sb::Params* params )
{
	rawContent->colorImage = cv::Mat::zeros( params->COLOR_FRAME_SIZE, CV_8UC3 );
}

void sb::release( sb::RawContent* rawContent ) {}
