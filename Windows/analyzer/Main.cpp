#include "../Classes/calculator/Calculator.h"
#include "../Classes/collector/Collector.h"
#include "../Classes/analyzer/Analyzer.h"
#include "../Classes/Timer.h"
#include <conio.h>

int init( sb::Collector& collector,
          sb::Calculator& calculator,
          sb::Analyzer& analyzer,
          const sb::Params& params );

void test( const sb::Calculator& calculator,
           const sb::RawContent& rawContent,
           const sb::FrameInfo& frameInfo,
           const sb::RoadInfo& roadInfo );

void release( sb::Collector& collector,
              sb::Calculator& calculator,
              sb::Analyzer& analyzer );

int main( const int argc, const char** argv )
{
	if ( argc < 2 ) {
		std::cerr << "Can't find argument for Params path" << std::endl;
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

	sb::FrameInfo frameInfo;
	frameInfo.create( params );

	sb::RoadInfo roadInfo;
	roadInfo.create( params );

	// Main components
	sb::Collector collector;
	sb::Calculator calculator;
	sb::Analyzer analyzer;

	// Init components
	if ( init( collector, calculator, analyzer, params ) < 0 ) {
		std::cerr << "Init failed." << std::endl;
		return -1;
	}

	cv::namedWindow( "Ego-view", CV_WINDOW_KEEPRATIO );
	cv::namedWindow( "Birdeye-view", CV_WINDOW_KEEPRATIO );

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
		if ( collector.collect( rawContent ) < 0 ) {
			std::cerr << "Collector collect failed." << std::endl;
			break;
		}
		std::cout << "Collector: " << timer.milliseconds( "collector" ) << "ms." << std::endl;

		////// </Collector> /////

		////// <Calculator> /////

		timer.reset( "calculator" );
		if ( calculator.calculate( rawContent, frameInfo ) < 0 ) {
			std::cerr << "Calculator calculate failed." << std::endl;
			break;
		}
		std::cout << "Calculator: " << timer.milliseconds( "calculator" ) << "ms." << std::endl;

		////// </Calculator> /////

		////// <Analyzer> /////

		timer.reset( "analyzer" );
		if ( analyzer.analyze( frameInfo, roadInfo ) ) {
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
		test( calculator, rawContent, frameInfo, roadInfo );
		///// <Test> /////

		///// <Result-writer> /////
		if( colorAvi.isOpened() && !rawContent.getColorImage().empty() ) {
			colorAvi << rawContent.getColorImage();
		}
		
		if( roadInfoStream.isOpened() ) {
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

	return 0;
}

int init( sb::Collector& collector,
          sb::Calculator& calculator,
          sb::Analyzer& analyzer,
          const sb::Params& params )
{
	if ( collector.init( params ) < 0 ) {
		std::cerr << "Collector init failed." << std::endl;
		return -1;
	}

	if ( calculator.init( params ) < 0 ) {
		std::cerr << "Calculator init failed." << std::endl;
		return -1;
	}

	if ( analyzer.init( params ) < 0 ) {
		std::cerr << "Analyzer init failed." << std::endl;
		return -1;
	}

	return 0;
}

void test( const sb::Calculator& calculator,
           const sb::RawContent& rawContent,
           const sb::FrameInfo& frameInfo,
           const sb::RoadInfo& roadInfo )
{
	///// Init image /////
	const int N_SECTIONS = static_cast<int>(frameInfo.getSectionInfos().size());

	const cv::Size FRAME_SIZE = frameInfo.getColorImage().size();

	const cv::Size CAR_SIZE( 90, 120 );

	const cv::Size EXPAND_SIZE( 900, 700 );

	const cv::Point CAR_POSITION( FRAME_SIZE.width / 2,
	                              FRAME_SIZE.height );

	cv::Mat radarImage = cv::Mat::zeros( FRAME_SIZE.height + EXPAND_SIZE.height + CAR_SIZE.height,
	                                     FRAME_SIZE.width + EXPAND_SIZE.width,
	                                     CV_8UC3 );

	///// Calculate lane positions /////

	std::vector<cv::Point2d> leftKnots( N_SECTIONS + 1, cv::Point2d( 0, 0 ) );
	std::vector<cv::Point2d> rightKnots( N_SECTIONS + 1, cv::Point2d( 0, 0 ) );

	for ( int i = 0; i < N_SECTIONS + 1; i++ ) {
		leftKnots[i] = calculator.convertFromCoord( roadInfo.getLeftKnots()[i] );
		rightKnots[i] = calculator.convertFromCoord( roadInfo.getRightKnots()[i] );
	}

	// draw lane
	for ( int i = 0; i < N_SECTIONS; i++ ) {
		cv::line( radarImage,
		          leftKnots[i] + cv::Point2d( EXPAND_SIZE.width / 2, EXPAND_SIZE.height ),
		          leftKnots[i + 1] + cv::Point2d( EXPAND_SIZE.width / 2, EXPAND_SIZE.height ),
		          cv::Scalar( 255, 255, 255 ), 7 );
		cv::line( radarImage,
		          rightKnots[i] + cv::Point2d( EXPAND_SIZE.width / 2, EXPAND_SIZE.height ),
		          rightKnots[i + 1] + cv::Point2d( EXPAND_SIZE.width / 2, EXPAND_SIZE.height ),
		          cv::Scalar( 255, 255, 255 ), 7 );
	}

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

	cv::waitKey( 33 );
}

void release( sb::Collector& collector,
              sb::Calculator& calculator,
              sb::Analyzer& analyzer )
{
	calculator.release();

	collector.release();

	analyzer.release();
}
