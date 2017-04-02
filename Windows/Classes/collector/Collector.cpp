#include "Collector.h"

int sb::Collector::init( const sb::Params& params )
{
	// sample init

	_tempCap = cv::VideoCapture( "../../Debug/video-1.avi" );

	if ( !_tempCap.isOpened() ) {
		std::cerr << "Init stream failed." << std::endl;
		return -1;
	}

	return 0;
}

int sb::Collector::collect( sb::RawContent& rawContent )
{
	// sample collect

	cv::Mat colorImage;

	_tempCap >> colorImage;

	if ( colorImage.empty() ) {
		std::cerr << "Stream disconnected." << std::endl;
		return -1;
	}

	rawContent.setColorImage( colorImage );

	return 0;
}

void sb::Collector::release()
{
	// sample release

	_tempCap.release();
}
