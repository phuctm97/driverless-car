#include "Collector.h"

int sb::init( sb::Collector* collector, sb::Params* params )
{
	// sample init

<<<<<<< HEAD
	collector->tempCap = cv::VideoCapture( "../../Debug/sample-2.avi" );

	if ( !collector->tempCap.isOpened() ) {
=======
	_tempCap = cv::VideoCapture( "../../Debug/sample-4.mp4" );

	if ( !_tempCap.isOpened() ) {
>>>>>>> master
		std::cerr << "Init stream failed." << std::endl;
		return -1;
	}

	return 0;
}

int sb::collect( sb::Collector* collector, sb::RawContent* rawContent )
{
	// sample collect

	collector->tempCap >> rawContent->colorImage;

<<<<<<< HEAD
	if ( rawContent->colorImage.empty() ) {
=======
	_tempCap >> colorImage;

	if ( colorImage.empty() ) {
>>>>>>> master
		std::cerr << "Stream disconnected." << std::endl;
		return -1;
	}

	return 0;
}

void sb::release( sb::Collector* collector )
{
	// sample release
<<<<<<< HEAD
	collector->tempCap.release();
=======

	_tempCap.release();
>>>>>>> master
}
