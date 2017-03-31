#include "../Classes/collector/Collector.h"
#include "../Classes/Timer.h"
#include <conio.h>

int init( sb::Collector& collector,
          const sb::Params& params );

void release( sb::Collector& collector );

int main( const int argc, const char** argv )
{
	if ( argc < 2 ) {
		std::cerr << "Cann't find argument for Params path" << std::endl;
		return -1;
	}

	// Application parameters
	sb::Params params;
	params.load( argv[1] );

	// Timer for performance test
	sb::Timer timer;

	// Data sent&receive between components
	sb::RawContent rawContent;
	rawContent.create( params );

	// Main components
	sb::Collector collector;

	// Init components
	if ( init( collector, params ) < 0 ) {
		std::cerr << "Init failed." << std::endl;
		return -1;
	}

	// Pressed key
	char key = 0;

	// Timer
	int timerTickCount = 0;
	timer.reset( "entire-job" );

	while ( true ) {
		timer.reset( "total" );

		////// <Collector> /////

		timer.reset( "collector" );
		if ( collector.collect( rawContent ) < 0 ) {
			std::cerr << "Collector collect failed." << std::endl;
			break;
		}
		std::cout << "Collector: " << timer.milliseconds( "collector" ) << "ms." << std::endl;

		////// </Collector> /////

		///// <Test> //////

		cv::imshow( "Window", rawContent.getColorImage() );
		cv::waitKey( 33 );

		///// </Test> /////

		///// <Timer> /////
	
		std::cout << "Executed time: " << timer.milliseconds( "total" ) << ". " << "FPS: " << timer.fps( "total" ) << "." << std::endl;
		timerTickCount++;
	
		///// </Timer> /////

		///// <User interuption> /////
		key = static_cast<char>(_kbhit());

		if ( key == 'f' ) break;
		///// </User interuption> /////
	}

	// Performance conclusion
	if( timerTickCount > 0 ) {
		std::cout << std::endl << "Average executiton time: " << timer.milliseconds( "entire-job" ) / timerTickCount << "ms." << std::endl;
	}

	// Release components
	release( collector );
	return 0;
}

int init( sb::Collector& collector,
          const sb::Params& params )
{
	if ( collector.init( params ) < 0 ) {
		std::cerr << "Collector init failed." << std::endl;
		return -1;
	}

	return 0;
}

void release( sb::Collector& collector )
{
	collector.release();
}
