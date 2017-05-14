#include "Blob.h"

void sb::release( Blob* blob )
{
	if ( blob == nullptr ) return;

	for ( auto cit_childblob = blob->childBlobs.cbegin(); cit_childblob != blob->childBlobs.cend(); ++cit_childblob ) {
		sb::release( *cit_childblob );
		delete *cit_childblob;
	}
	blob->childBlobs.clear();
	blob->rows.clear();
}
