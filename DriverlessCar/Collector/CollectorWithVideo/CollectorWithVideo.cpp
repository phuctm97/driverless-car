#include "CollectorWithVideo.h"

sb::CollectorWithVideo::CollectorWithVideo( const std::string& fileName )
{
	_stream.open( fileName );

	std::cout << "CollectorWithVideo: video frame count: " << static_cast<int>(_stream.get( CV_CAP_PROP_FRAME_COUNT )) << std::endl;
}

int sb::CollectorWithVideo::init()
{
	if ( !_stream.isOpened() ) {
		std::cerr << "CollectorWithVideo: empty stream" << std::endl;
		return -1;
	}

	return 0;
}

int sb::CollectorWithVideo::collect( CollectData* collectData )
{
	_stream >> collectData->colorImage;

	if ( collectData->colorImage.empty() ) {
		std::cerr << "CollectorWithVideo: end of stream";
		return -1;
	}

	return 0;
}

void sb::CollectorWithVideo::release()
{
	_stream.release();
}
