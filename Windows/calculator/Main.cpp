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
	cv::Mat debugImage = frameInfo->bgrImage;
	cv::imshow( "Calculator", debugImage );
	cv::waitKey();

	cv::imshow( "Calculator", frameInfo->binImage );
	cv::waitKey();

	for ( auto cit_blob = frameInfo->blobs.cbegin(); cit_blob != frameInfo->blobs.cend(); ++cit_blob ) {
		cv::Mat img = debugImage.clone();

		sb::Blob* blob = *cit_blob;
		for ( auto cit_childblob = blob->childBlobs.cbegin(); cit_childblob != blob->childBlobs.cend(); ++cit_childblob ) {
			sb::Blob* childBlob = *cit_childblob;

			if ( childBlob->size == 0 ) continue;

			cv::rectangle( img, childBlob->box.tl(), childBlob->box.br(), cv::Scalar( 0, 0, 255 ), 1 );
			cv::circle( img, childBlob->origin, 3, cv::Scalar( 0, 255, 0 ), 2 );
		}

		cv::putText( img, std::to_string( blob->size ), cv::Point( frameInfo->bgrImage.cols / 2, 30 ),
		             cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar( 0, 255, 255 ), 2 );
		cv::imshow( "Calculator", img );
		cv::waitKey();
	}
}
