#include "../Classes/calculator/Calculator.h"
#include "../Classes/collector/Collector.h"
#include "../Classes/analyzer/Analyzer.h"
#include "../Classes/Timer.h"
#include <conio.h>
<<<<<<< HEAD

#define SB_DEBUG
=======
>>>>>>> master

int init( sb::Collector* collector,
          sb::Calculator* calculator,
          sb::Analyzer* analyzer,
          sb::Params* params );

<<<<<<< HEAD
=======
void test( const sb::Calculator& calculator,
           const sb::RawContent& rawContent,
           const sb::FrameInfo& frameInfo,
           const sb::RoadInfo& roadInfo );

void release( sb::Collector& collector,
              sb::Calculator& calculator,
              sb::Analyzer& analyzer );

>>>>>>> master
int main( const int argc, const char** argv )
{
	if ( argc < 2 ) {
		std::cerr << "Can't find argument for Params path" << std::endl;
		return -1;
	}

	// Application parameters
<<<<<<< HEAD
	sb::Params* params = new sb::Params;
	sb::load( params, argv[1] );
=======
	sb::Params params;
	params.load( argv[1] );
>>>>>>> master

	// Timer for performance test
	sb::Timer timer;

	// Data sent&receive between components
<<<<<<< HEAD
	sb::RawContent* rawContent = new sb::RawContent;
	sb::create( rawContent, params );
=======
	sb::RawContent rawContent;
	rawContent.create( params );
>>>>>>> master

	sb::FrameInfo* frameInfo = new sb::FrameInfo;
	sb::create( frameInfo, params );

	sb::RoadInfo* roadInfo = new sb::RoadInfo;
	sb::create( roadInfo, params );

	// Main components
<<<<<<< HEAD
	sb::Collector* collector = new sb::Collector;
	sb::Calculator* calculator = new sb::Calculator;
	sb::Analyzer* analyzer = new sb::Analyzer;
=======
	sb::Collector collector;
	sb::Calculator calculator;
	sb::Analyzer analyzer;
>>>>>>> master

	// Init components
	if ( init( collector, calculator, analyzer, params ) < 0 ) {
		std::cerr << "Init failed." << std::endl;
		return -1;
	}

	// Pressed key
	char key = 0;
	std::cout << "Enter 's' to start! ";
<<<<<<< HEAD
	while ( key != 's' ) {
		fflush( stdin );
		std::cin >> key;
	}
=======
	while ( key != 's' ) std::cin >> key;
>>>>>>> master

	// Timer
	int timerTickCount = 0;
	timer.reset( "entire-job" );

	///// <Result-writer> /////
	cv::VideoWriter colorAvi;
<<<<<<< HEAD
	cv::Mat colorVideoFrame;
	if ( argc > 2 ) {
		colorAvi.open( argv[2], CV_FOURCC( 'M', 'J', 'P', 'G' ), 15, params->COLOR_FRAME_SIZE );
	}
=======
	if ( argc > 2 ) {
		colorAvi.open( argv[2], CV_FOURCC( 'M', 'J', 'P', 'G' ), 15, params.COLOR_FRAME_SIZE );
	}

	cv::FileStorage roadInfoStream;
	if ( argc > 3 ) {
		roadInfoStream.open( argv[3], cv::FileStorage::WRITE );
	}
	int frameCount = 0;
>>>>>>> master
	///// </Result-writer> /////

	while ( true ) {
		timer.reset( "total" );

		////// <Collector> /////

		timer.reset( "collector" );
<<<<<<< HEAD
		if ( sb::collect( collector, rawContent ) < 0 ) {
=======
		if ( collector.collect( rawContent ) < 0 ) {
>>>>>>> master
			std::cerr << "Collector collect failed." << std::endl;
			break;
		}
		std::cout << "Collector: " << timer.milliseconds( "collector" ) << "ms." << std::endl;

		////// </Collector> /////

<<<<<<< HEAD
		if ( colorAvi.isOpened() ) {
			cv::flip( rawContent->colorImage, colorVideoFrame, 1 );
		}

		////// <Calculator> /////

		timer.reset( "calculator" );
		if ( sb::calculate( calculator, rawContent, frameInfo ) < 0 ) {
=======
		////// <Calculator> /////

		timer.reset( "calculator" );
		if ( calculator.calculate( rawContent, frameInfo ) < 0 ) {
>>>>>>> master
			std::cerr << "Calculator calculate failed." << std::endl;
			break;
		}
		std::cout << "Calculator: " << timer.milliseconds( "calculator" ) << "ms." << std::endl;

		////// </Calculator> /////

		////// <Analyzer> /////

		timer.reset( "analyzer" );
<<<<<<< HEAD
		if ( sb::analyze( analyzer, frameInfo, roadInfo ) ) {
=======
		if ( analyzer.analyze( frameInfo, roadInfo ) ) {
>>>>>>> master
			std::cerr << "Analyzer analyze failed." << std::endl;
			break;
		}
		std::cout << "Analyzer: " << timer.milliseconds( "analyzer" ) << "ms." << std::endl;

		////// </Analyzer> /////

		///// <Timer> /////
		std::cout << "Executed time: " << timer.milliseconds( "total" ) << ". " << "FPS: " << timer.fps( "total" ) << "." << std::endl;
		timerTickCount++;
		///// </Timer> /////

<<<<<<< HEAD
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
=======
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
>>>>>>> master
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

<<<<<<< HEAD
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
=======
	// Release components
	release( collector, calculator, analyzer );

	///// <Result-writer> /////
	colorAvi.release();

	roadInfoStream << "frame_count" << frameCount;
	roadInfoStream.release();
	///// </Result-writer> /////
>>>>>>> master

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
<<<<<<< HEAD
=======

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

	cv::imshow( "Ego-view", rawContent.getColorImage() );

	cv::imshow( "Bird-eye view", radarImage );

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
>>>>>>> master
