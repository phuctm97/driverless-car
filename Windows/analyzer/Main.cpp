#include "../Classes/calculator/Calculator.h"
#include "../Classes/collector/Collector.h"
#include "../Classes/analyzer/Analyzer.h"
#include "../Classes/Timer.h"

int init( sb::Collector& collector,
          sb::Calculator& calculator,
          sb::Analyzer& analyzer,
          const sb::Params& params );

void test( const sb::RawContent& rawContent,
           const sb::FrameInfo& frameInfo,
           const sb::RoadInfo& roadInfo,
           const sb::Params& params );

void release( sb::Collector& collector,
              sb::Calculator& calculator,
              sb::Analyzer& analyzer );

int main()
{
	// Parameters container for every component
	sb::Params params;
	params.load( PARAMS_PATH );

	// Timer for performance test
	sb::Timer timer;

	// Data sent&receive bewteen components
	sb::RawContent rawContent;
	sb::FrameInfo frameInfo;
	sb::RoadInfo roadInfo;

	// Main Components
	sb::Collector collector;
	sb::Calculator calculator;
	sb::Analyzer analyzer;

	// Init components
	if ( init( collector, calculator, analyzer, params ) < 0 ) {
		std::cerr << "Init failed." << std::endl;
		return -1;
	}

	// Initial values
	rawContent.create( params );
	frameInfo.create( params );
	roadInfo.create( params );

	while ( true ) {

		timer.reset( "total" );

		if ( collector.collect( rawContent ) < 0 ) {
			std::cerr << "Collector collects failed." << std::endl;
			break;
		}

		if ( calculator.calculate( rawContent, frameInfo ) < 0 ) {
			std::cerr << "Calculator calculates failed." << std::endl;
			break;
		}

		if ( analyzer.analyze( frameInfo, roadInfo ) ) {
			std::cerr << "Analyzer analyzes failed." << std::endl;
			break;
		}

		test( rawContent, frameInfo, roadInfo, params );

		std::cout
				<< "Executed time: " << timer.milliseconds( "total" ) << ". "
				<< "FPS: " << timer.fps( "total" ) << std::endl;

		if ( cv::waitKey( MAX(1, 66 - timer.milliseconds("total")) ) == KEY_TO_ESCAPE ) break;
	}

	release( collector, calculator, analyzer );
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

void test( const sb::RawContent& rawContent,
           const sb::FrameInfo& frameInfo,
           const sb::RoadInfo& roadInfo,
           const sb::Params& params )
{
	const cv::Size FRAME_SIZE = frameInfo.getColorImage().size();

	const cv::Point CAR_POSITION( FRAME_SIZE.width / 2, FRAME_SIZE.height );

	const cv::Size CAR_SIZE( 50, 90 );

	const cv::Size EXPAND_SIZE( 300, 500 );

	const sb::Line TOP_LINE( cv::Point2d( 0, 0 ) + cv::Point2d( EXPAND_SIZE.width / 2, EXPAND_SIZE.height / 2 ),
	                         cv::Point2d( 1, 0 ) + cv::Point2d( EXPAND_SIZE.width / 2, EXPAND_SIZE.height / 2 ) );

	cv::Mat radarImage = cv::Mat::zeros(
	                                    FRAME_SIZE.height + EXPAND_SIZE.height,
	                                    FRAME_SIZE.width + EXPAND_SIZE.width,
	                                    CV_8UC3
	                                   );

	cv::rectangle( radarImage,
	               CAR_POSITION - cv::Point( CAR_SIZE.width / 2, 0 ) + cv::Point( EXPAND_SIZE.width / 2, EXPAND_SIZE.height / 2 ),
	               CAR_POSITION + cv::Point( CAR_SIZE.width / 2, CAR_SIZE.height ) + cv::Point( EXPAND_SIZE.width / 2, EXPAND_SIZE.height / 2 ),
	               cv::Scalar( 0, 0, 255 ), -1 );
	cv::rectangle( radarImage,
	               CAR_POSITION - cv::Point( CAR_SIZE.width / 2, 0 ) + cv::Point( EXPAND_SIZE.width / 2, EXPAND_SIZE.height / 2 ),
	               CAR_POSITION + cv::Point( CAR_SIZE.width / 2, CAR_SIZE.height ) + cv::Point( EXPAND_SIZE.width / 2, EXPAND_SIZE.height / 2 ),
	               cv::Scalar( 0, 255, 255 ), 4 );

	cv::Point2d positionOfLeftLane = cv::Point2d( ((roadInfo.getPositionOfLeftLane().x + 1) / 2.0) * FRAME_SIZE.width,
	                                              CAR_POSITION.y - 20 );
	cv::Point2d positionOfRightLane = cv::Point2d( ((roadInfo.getPositionOfRightLane().x + 1) / 2.0) * FRAME_SIZE.width,
	                                               CAR_POSITION.y - 20 );

	cv::circle( radarImage,
	            positionOfLeftLane + cv::Point2d( EXPAND_SIZE.width / 2, EXPAND_SIZE.height / 2 ),
	            5, cv::Scalar( 255, 255, 255 ), -1 );

	cv::circle( radarImage,
	            positionOfRightLane + cv::Point2d( EXPAND_SIZE.width / 2, EXPAND_SIZE.height / 2 ),
	            5, cv::Scalar( 255, 255, 255 ), -1 );

	cv::imshow( "Full Camera", rawContent.getColorImage() );

	cv::imshow( "Radar", radarImage );

	cv::waitKey();
}

void release( sb::Collector& collector,
              sb::Calculator& calculator,
              sb::Analyzer& analyzer )
{
	calculator.release();

	collector.release();

	analyzer.release();
}
