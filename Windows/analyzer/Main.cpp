#include "../Classes/calculator/Calculator.h"
#include "../Classes/collector/Collector.h"
#include "../Classes/analyzer/Analyzer.h"
#include "../Classes/Timer.h"
#include <conio.h>

int init( sb::Collector* collector,
          sb::Calculator* calculator,
          sb::Analyzer* analyzer,
          sb::Params* params );

void test( sb::RawContent* rawContent,
           sb::FrameInfo* frameInfo,
           sb::RoadInfo* roadInfo );

void release( sb::Collector* collector,
              sb::Calculator* calculator,
              sb::Analyzer* analyzer );

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
	sb::create( roadInfo params );

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
	while ( key != 's' ) std::cin >> key;

	// Timer
	int timerTickCount = 0;
	timer.reset( "entire-job" );

	///// Debug //////
	/*cv::namedWindow( "Ego-view" );
	cv::namedWindow( "Birdeye-view" );
	cv::waitKey();*/

	///// <Result-writer> /////
	cv::VideoWriter colorAvi;
	if ( argc > 2 ) {
		colorAvi.open( argv[2], CV_FOURCC( 'M', 'J', 'P', 'G' ), 15, params.COLOR_FRAME_SIZE );
	}

	cv::FileStorage roadInfoStream;
	if ( argc > 3 ) {
		roadInfoStream.open( argv[3], cv::FileStorage::WRITE );
	}
	int frameCount = 0;
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

		///// <Test> /////
		test( rawContent, frameInfo, roadInfo );
		///// <Test> /////

		///// <Result-writer> /////
		if ( colorAvi.isOpened() && !rawContent->colorImage.empty() ) {
			colorAvi << rawContent->colorImage;
		}

		if ( roadInfoStream.isOpened() ) {
			std::stringstream stringBuilder;
			stringBuilder << "road_" << frameCount++;
			roadInfoStream << stringBuilder.str() << roadInfo;
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
	release( collector, calculator, analyzer );

	///// <Result-writer> /////
	colorAvi.release();

	roadInfoStream << "frame_count" << frameCount;
	roadInfoStream.release();
	///// </Result-writer> /////

	delete params;
	delete rawContent;
	sb::clear( frameInfo );
	delete frameInfo;
	delete roadInfo;
	delete collector;
	delete calculator;
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

void test( sb::RawContent* rawContent,
           sb::FrameInfo* frameInfo,
           sb::RoadInfo* roadInfo )
{
	/*///// Init image /////
	const cv::Size FRAME_SIZE = frameInfo.getColorImage().size();

	const cv::Size CAR_SIZE( 90, 120 );

	const cv::Size EXPAND_SIZE( 900, 700 );

	const cv::Point CAR_POSITION( FRAME_SIZE.width / 2,
	                              FRAME_SIZE.height );

	cv::Mat radarImage = cv::Mat::zeros( FRAME_SIZE.height + EXPAND_SIZE.height + CAR_SIZE.height,
	                                     FRAME_SIZE.width + EXPAND_SIZE.width,
	                                     CV_8UC3 );

	///// Calculate lane positions /////

	cv::Point2d target = roadInfo.getTarget();

	cv::line( radarImage,
	          calculator.convertFromCoord( target ) + cv::Point2d( EXPAND_SIZE.width / 2, EXPAND_SIZE.height ),
	          CAR_POSITION + cv::Point( EXPAND_SIZE.width / 2, EXPAND_SIZE.height ),
	          cv::Scalar( 0, 255, 0 ), 3 );
	cv::circle( radarImage,
	            calculator.convertFromCoord( target ) + cv::Point2d( EXPAND_SIZE.width / 2, EXPAND_SIZE.height ),
	            5, cv::Scalar( 0, 0, 255 ), -1 );
	cv::circle( radarImage,
	            calculator.convertFromCoord( target ) + cv::Point2d( EXPAND_SIZE.width / 2, EXPAND_SIZE.height ),
	            5, cv::Scalar( 0, 255, 0 ), 3 );

	std::stringstream stringBuilder;
	stringBuilder << "(" << target.x << ", " << target.y << ")";
	cv::putText( radarImage, stringBuilder.str(),
	             calculator.convertFromCoord( target ) + cv::Point2d( EXPAND_SIZE.width / 2, EXPAND_SIZE.height ) - cv::Point2d( 20, 20 ),
	             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 255, 255, 255 ) );

	// draw vehicle
	cv::rectangle( radarImage,
	               CAR_POSITION - cv::Point( CAR_SIZE.width / 2, 0 ) + cv::Point( EXPAND_SIZE.width / 2, EXPAND_SIZE.height ),
	               CAR_POSITION + cv::Point( CAR_SIZE.width / 2, CAR_SIZE.height ) + cv::Point( EXPAND_SIZE.width / 2, EXPAND_SIZE.height ),
	               cv::Scalar( 0, 0, 255 ), -1 );
	cv::rectangle( radarImage,
	               CAR_POSITION - cv::Point( CAR_SIZE.width / 2, 0 ) + cv::Point( EXPAND_SIZE.width / 2, EXPAND_SIZE.height ),
	               CAR_POSITION + cv::Point( CAR_SIZE.width / 2, CAR_SIZE.height ) + cv::Point( EXPAND_SIZE.width / 2, EXPAND_SIZE.height ),
	               cv::Scalar( 0, 255, 255 ), 4 );

	cv::imshow( "Ego-view", frameInfo.getColorImage() );

	cv::imshow( "Birdeye-view", radarImage );

	cv::waitKey( 500 );*/
}

void release( sb::Collector* collector,
              sb::Calculator* calculator,
              sb::Analyzer* analyzer )
{
	sb::release( collector );

	sb::release( calculator );

	sb::release( analyzer );
}
