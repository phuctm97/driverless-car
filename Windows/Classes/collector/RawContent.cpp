#include "RawContent.h"

void sb::construct( sb::RawContent*& rawContent )
{
	rawContent = new RawContent;
}

void sb::destruct( sb::RawContent*& rawContent )
{
	delete rawContent;
	rawContent = nullptr;
}

void sb::create( sb::RawContent* rawContent, sb::Params* params )
{
	rawContent->colorImage = cv::Mat::zeros( params->COLOR_FRAME_SIZE, CV_8UC3 );
}
