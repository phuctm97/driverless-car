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
<<<<<<< HEAD
=======

double sb::FrameInfo::convertYFromCoord( double y ) const
{
	return _colorImage.rows - (y / _convertCoordCoef);
}

cv::Point2d sb::FrameInfo::convertFromCoord( const cv::Point2d& point ) const
{
	return cv::Point2d(
	                   convertXFromCoord( point.x ),
	                   convertYFromCoord( point.x )
	                  );
}

double sb::FrameInfo::convertToRotation( double angle ) const
{
	return 90 - angle;
}

double sb::FrameInfo::convertFromRotation( double rotation ) const
{
	return 90 - rotation;
}*/

>>>>>>> master
