#include "FrameInfo.h"

void sb::create( sb::FrameInfo* frameInfo, sb::Params* params ) {}

void sb::clear( sb::FrameInfo* frameInfo )
{
	frameInfo->colorImage.release();

	for( auto it_section = frameInfo->imageSections.begin(); it_section != frameInfo->imageSections.end(); ++it_section ) {
		sb::clear( *it_section );
		delete *it_section;
	}
	frameInfo->imageSections.clear();
}
