#include "../Classes/calculator/Calculator.h"
#include "../Classes/collector/Collector.h"
#include "../Classes/Timer.h"
#include <conio.h>

int init( sb::Collector* collector,
          sb::Calculator* calculator,
          sb::Params* params );

void test( sb::Collector* collector,
           sb::Calculator* calculator,
           sb::RawContent* rawContent,
           sb::FrameInfo* frameInfo );

int main( const int argc, const char** argv )
{
	if ( argc < 2 ) {
		std::cerr << "Can't find argument for Params path" << std::endl;
		return -1;
	}

	std::srand( static_cast<unsigned int>(time( 0 )) );

	// Application parameters
	sb::Params* params = new sb::Params;
	sb::load( params, argv[1] );

	// Timer for performance test
	sb::Timer timer;

	// Data sent&receive between components
	sb::RawContent* rawContent = new sb::RawContent;
	sb::create( rawContent, params );

	sb::FrameInfo* frameInfo = new sb::FrameInfo;
	sb::create( frameInfo, params );

	// Main components
	sb::Collector* collector = new sb::Collector;

	sb::Calculator* calculator = new sb::Calculator;

	// Init components
	if ( init( collector, calculator, params ) < 0 ) {
		std::cerr << "Init failed." << std::endl;
		return -1;
	}

	// Pressed key
	char key = 0;
	std::cout << "Enter 's' to start! ";
	while ( key != 's' ) std::cin >> key;

	// Timer
	int timerTickCount = 0;
	timer.reset( "entire-job" );

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

		////// <Calculator> /////

		timer.reset( "calculator" );
		if ( sb::calculate( calculator, rawContent, frameInfo ) < 0 ) {
			std::cerr << "Calculator calculate failed." << std::endl;
			break;
		}
		std::cout << "Calculator: " << timer.milliseconds( "calculator" ) << "ms." << std::endl;

		////// </Calculator> /////

		///// <Timer> /////
		std::cout << "Executed time: " << timer.milliseconds( "total" ) << ". " << "FPS: " << timer.fps( "total" ) << "." << std::endl;
		timerTickCount++;
		///// </Timer> /////

		///// <Test> /////
		test( collector, calculator, rawContent, frameInfo );
		///// </Test> /////

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
	sb::release( params );
	delete params;

	sb::release( rawContent );
	delete rawContent;

	sb::release( frameInfo );
	delete frameInfo;

	sb::release( collector );
	delete collector;

	sb::release( calculator );
	delete calculator;

	return 0;
}

int init( sb::Collector* collector,
          sb::Calculator* calculator,
          sb::Params* params )
{
	if ( sb::init( collector, params ) < 0 ) {
		std::cerr << "Collector init failed." << std::endl;
		return -1;
	}

	if ( sb::init( calculator, params ) < 0 ) {
		std::cerr << "Calculator init failed." << std::endl;
		return -1;
	}

	return 0;
}

void test( sb::Collector* collector,
           sb::Calculator* calculator,
           sb::RawContent* rawContent,
           sb::FrameInfo* frameInfo )
{
	cv::Mat images[2];
	images[0] = frameInfo->bgrImage.clone();
	images[1] = frameInfo->binImage.clone();
	for( int i = 0; i < 2; i++ ) {
		cv::imshow( "Image " + std::to_string( i ), images[i] );
	}
	cv::waitKey();

	for ( auto it_section = frameInfo->imageSections.cbegin(); it_section != frameInfo->imageSections.cend(); ++it_section ) {
		sb::Section* section = *it_section;

		for ( auto it_blob = section->blobs.cbegin(); it_blob != section->blobs.cend(); ++it_blob ) {
			sb::Blob* blob = *it_blob;
			for ( auto pixel : blob->pixels ) {
				for ( int i = 0; i < 2; i++ ) {
					cv::circle( images[i], pixel, 1, cv::Scalar( blob->bgr[0], blob->bgr[1], blob->bgr[2] ), 1 );
				}
			}
		}
	}

	for ( int i = 0; i < 2; i++ ) {
		cv::imshow( "Image " + std::to_string( i ), images[i] );
	}
	cv::waitKey();
}
