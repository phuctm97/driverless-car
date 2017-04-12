#include "FrameInfo.h"

void sb::create( sb::FrameInfo* frameInfo, sb::Params* params ) {}

void sb::release( sb::FrameInfo* frameInfo )
{
	frameInfo->bgrImage.release();
	frameInfo->binImage.release();
	frameInfo->edgImage.release();

	for( auto it_section = frameInfo->imageSections.begin(); it_section != frameInfo->imageSections.end(); ++it_section ) {
		sb::release( *it_section );
		delete *it_section;
	}
	frameInfo->imageSections.clear();
}
