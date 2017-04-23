#include "../Classes/calculator/Calculator.h"
#include "../Classes/collector/Collector.h"
#include "../Classes/analyzer/Analyzer.h"
#include "../Classes/Timer.h"
#include <conio.h>

#define SB_DEBUG

int init( sb::Collector* collector,
          sb::Calculator* calculator,
          sb::Analyzer* analyzer,
          sb::Params* params );

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

	sb::FrameInfo* frameInfo = new sb::FrameInfo;
	sb::create( frameInfo, params );

	sb::RoadInfo* roadInfo = new sb::RoadInfo;
	sb::create( roadInfo, params );

	// Main components
	sb::Collector* collector = new sb::Collector;
	sb::Calculator* calculator = new sb::Calculator;
	sb::Analyzer* analyzer = new sb::Analyzer;

	// Init components
	if ( init( collector, calculator, analyzer, params ) < 0 ) {
		std::cerr << "Init failed." << std::endl;
		return -1;
	}

	// Pressed key
	char key = 0;
	std::cout << "Enter 's' to start! ";
	while ( key != 's' ) {
		fflush( stdin );
		std::cin >> key;
	}

	// Timer
	int timerTickCount = 0;
	timer.reset( "entire-job" );

	///// <Result-writer> /////
	cv::VideoWriter colorAvi;
	cv::Mat colorVideoFrame;
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

		if ( colorAvi.isOpened() ) {
			cv::flip( rawContent->colorImage, colorVideoFrame, 1 );
		}

		////// <Calculator> /////

		timer.reset( "calculator" );
		if ( sb::calculate( calculator, rawContent, frameInfo ) < 0 ) {
			std::cerr << "Calculator calculate failed." << std::endl;
			break;
		}
		std::cout << "Calculator: " << timer.milliseconds( "calculator" ) << "ms." << std::endl;

		////// </Calculator> /////

		////// <Analyzer> /////

		timer.reset( "analyzer" );
		if ( sb::analyze( analyzer, frameInfo, roadInfo ) ) {
			std::cerr << "Analyzer analyze failed." << std::endl;
			break;
		}
		std::cout << "Analyzer: " << timer.milliseconds( "analyzer" ) << "ms." << std::endl;

		////// </Analyzer> /////

		///// <Timer> /////
		std::cout << "Executed time: " << timer.milliseconds( "total" ) << ". " << "FPS: " << timer.fps( "total" ) << "." << std::endl;
		timerTickCount++;
		///// </Timer> /////

		///// <Result-writer> /////
		if ( colorAvi.isOpened() && !colorVideoFrame.empty() ) {
			if ( analyzer->roadState == sb::RoadState::UNKNOWN ) {
				cv::putText( colorVideoFrame, "Failed!",
				             cv::Point( frameInfo->bgrImage.cols / 3, frameInfo->bgrImage.rows / 2 ),
				             cv::FONT_HERSHEY_PLAIN, 3, cv::Scalar( 0, 0, 255 ), 3 );
			}
			else {
				for ( auto cit_knot = analyzer->knots.cbegin(); cit_knot != analyzer->knots.cend(); ++cit_knot ) {
					if ( cit_knot->first.type > 0 ) {
						cv::circle( colorVideoFrame, cit_knot->first.position + params->CROP_BOX.tl(), 4, cv::Scalar( 0, 255, 0 ), 2 );
					}
					if ( cit_knot->second.type > 0 ) {
						cv::circle( colorVideoFrame, cit_knot->second.position + params->CROP_BOX.tl(), 4, cv::Scalar( 0, 255, 0 ), 2 );
					}
				}
				cv::circle( colorVideoFrame, roadInfo->target + params->CROP_BOX.tl(), 12, cv::Scalar( 0, 0, 0 ), -1 );
				cv::line( colorVideoFrame,
				          roadInfo->target - cv::Point( 7, 0 ) + params->CROP_BOX.tl(),
				          roadInfo->target + cv::Point( 7, 0 ) + params->CROP_BOX.tl(),
				          cv::Scalar( 0, 0, 255 ), 2 );
				cv::line( colorVideoFrame,
				          roadInfo->target - cv::Point( 0, 7 ) + params->CROP_BOX.tl(),
				          roadInfo->target + cv::Point( 0, 7 ) + params->CROP_BOX.tl(),
				          cv::Scalar( 0, 0, 255 ), 2 );
			}

			colorAvi << colorVideoFrame;
			cv::imshow( "Analyzer", colorVideoFrame );
			cv::waitKey( 1000 / 15 );
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

	///// <Result-writer> /////
	colorAvi.release();
	///// </Result-writer> /////

	// Release components
	sb::release( params );
	delete params;

	sb::release( rawContent );
	delete rawContent;

	sb::release( frameInfo );
	delete frameInfo;

	sb::release( roadInfo );
	delete roadInfo;

	sb::release( collector );
	delete collector;

	sb::release( calculator );
	delete calculator;

	sb::release( analyzer );
	delete analyzer;

	return 0;
}

int init( sb::Collector* collector,
          sb::Calculator* calculator,
          sb::Analyzer* analyzer,
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

	if ( sb::init( analyzer, params ) < 0 ) {
		std::cerr << "Analyzer init failed." << std::endl;
		return -1;
	}

	return 0;
}
