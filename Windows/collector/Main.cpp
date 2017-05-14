#include "../Classes/collector/Collector.h"
#include "../Classes/Timer.h"
#include <conio.h>

int main( const int argc, const char** argv )
{
	if ( argc < 2 ) {
		std::cerr << "Can't find argument for Params path" << std::endl;
		return -1;
	}

	// Application parameters
	sb::Params* params = new sb::Params;
	sb::load( params, argv[1] );

	// Timer for performance test
	sb::Timer timer;

	// Data sent&receive between components
	sb::RawContent* rawContent = new sb::RawContent;
	sb::create( rawContent, params );

	// Main components
	sb::Collector* collector = new sb::Collector;

	// Init components
	if( sb::init( collector, params ) < 0 ) {
		std::cerr << "Collector init failed." << std::endl;
		return -1;
	}

	// Pressed key
	char key = 0;
	std::cout << "Enter 's' to start! ";
	while ( key != 's' ) std::cin >> key;

	// Timer
	int timerTickCount = 0;
	timer.reset( "entire-job" );

	///// <Result-writer> /////
	cv::VideoWriter colorAvi;
	if ( argc > 2 ) {
		colorAvi.open( argv[2], CV_FOURCC( 'M', 'J', 'P', 'G' ), 15, params->COLOR_FRAME_SIZE );
	}
	///// </Result-writer> /////

	while ( true ) {
		timer.reset( "total" );

		////// <Collector> /////

		timer.reset( "collector" );
		if ( sb::collect( collector, rawContent ) < 0 ) {
			std::cerr << "Collector collect failed." << std::endl;
			break;
		}
		std::cout << "Collector: " << timer.milliseconds( "collector" ) << "ms." << std::endl;

		////// </Collector> /////

		///// <Timer> /////
		std::cout << "Executed time: " << timer.milliseconds( "total" ) << ". " << "FPS: " << timer.fps( "total" ) << "." << std::endl;
		timerTickCount++;
		///// </Timer> /////

		///// <Test> //////

		cv::imshow( "Window", rawContent->colorImage );
		cv::waitKey( 33 );

		///// </Test> /////

		///// <Result-writer> /////
		if ( colorAvi.isOpened() && !rawContent->colorImage.empty() ) {
			colorAvi << rawContent->colorImage;
		}
		///// </Result-writer> /////

		///// <User interuption> /////
		key = static_cast<char>(_kbhit());
		if ( key == 'f' ) break;
		///// </User interuption> /////
	}

	// Performance conclusion
	if ( timerTickCount > 0 ) {
		std::cout << std::endl << "Average executiton time: " << timer.milliseconds( "entire-job" ) / timerTickCount << "ms." << std::endl;
	}

	// Release components
	sb::release( params ); delete params;
	sb::release( rawContent ); delete rawContent;
	sb::release( collector );	delete collector;

	///// <Result-writer> /////
	colorAvi.release();
	///// </Result-writer> /////



	return 0;
}