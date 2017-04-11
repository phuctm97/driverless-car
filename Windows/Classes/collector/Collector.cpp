#include "Collector.h"

void sb::construct( sb::Collector*& collector )
{
	collector = new Collector;
}

void sb::destruct( sb::Collector*& collector )
{
	delete collector;
	collector = nullptr;
}

int sb::init( sb::Collector* collector, sb::Params* params )
{
	// sample init

	collector->tempCap = cv::VideoCapture( "../../Debug/sample-1.avi" );

	if ( !collector->tempCap.isOpened() ) {
		std::cerr << "Init stream failed." << std::endl;
		return -1;
	}

	return 0;
}

int sb::collect( sb::Collector* collector, sb::RawContent* rawContent )
{
	// sample collect

	collector->tempCap >> rawContent->colorImage;

	if ( rawContent->colorImage.empty() ) {
		std::cerr << "Stream disconnected." << std::endl;
		return -1;
	}

	return 0;
}

void sb::release( sb::Collector* collector )
{
	// sample release

	collector->tempCap.release();
}
