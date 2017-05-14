#include "CalculateData.h"

void sb::release( CalculateData* calculateData, bool releaseBlobs )
{
	if( calculateData == nullptr ) return;

	calculateData->bgrImage.release();
	calculateData->binImage.release();

	if( releaseBlobs ) {
		for( auto it_blob = calculateData->blobs.begin(); it_blob != calculateData->blobs.end(); ++it_blob ) {
			if( *it_blob != nullptr ) {
				sb::release( *it_blob );
				delete *it_blob;
			}
		}
	}

	calculateData->blobs.clear();
}
