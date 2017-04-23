#include "FrameInfo.h"

void sb::create( sb::FrameInfo* frameInfo, sb::Params* params ) {}

void sb::release( sb::FrameInfo* frameInfo, bool releaseBlobs )
{
	if ( frameInfo == nullptr ) return;

	frameInfo->bgrImage.release();
	frameInfo->binImage.release();
	frameInfo->edgImage.release();

	if ( releaseBlobs ) {
		for ( auto it_blob = frameInfo->blobs.begin(); it_blob != frameInfo->blobs.end(); ++it_blob ) {
			if ( *it_blob != nullptr ) {
				sb::release( *it_blob );
				delete *it_blob;
			}
		}
	}

	frameInfo->blobs.clear();
}
