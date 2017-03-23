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
	sb::Params params;
	params.load( PARAMS_PATH );

	sb::Timer timer;

	sb::RawContent rawContent;
	sb::FrameInfo frameInfo;
	sb::RoadInfo roadInfo;

	sb::Collector collector;
	sb::Calculator calculator;
	sb::Analyzer analyzer;

	if ( init( collector, calculator, analyzer, params ) < 0 ) {
		std::cerr << "Init failed." << std::endl;
		return -1;
	}

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
	int expandHeight = 200;

	cv::Mat mat1, mat2;
	mat1 = cv::Mat( frameInfo.getColorImage().rows + expandHeight,
	                frameInfo.getColorImage().cols,
	                CV_8UC3 );
	mat2 = cv::Mat( frameInfo.getColorImage().rows + expandHeight,
	                frameInfo.getColorImage().cols,
	                CV_8UC3 );

	for ( const auto& line: frameInfo.getLines() ) {
		cv::line( mat1,
		          line.getStartingPoint() + cv::Point2d( 0, expandHeight ),
		          line.getEndingPoint() + cv::Point2d( 0, expandHeight ),
		          cv::Scalar( 0, 255, 0 ) );
	}

	for ( const auto& line : frameInfo.getWarpedLines() ) {
		cv::line( mat2,
		          line.getStartingPoint() + cv::Point2d( 0, expandHeight ),
		          line.getEndingPoint() + cv::Point2d( 0, expandHeight ),
		          cv::Scalar( 0, 255, 0 ) );
	}

	cv::imshow( "Color", frameInfo.getColorImage() );
	cv::imshow( "Lines", mat1 );
	cv::imshow( "Warped Lines", mat2 );
}

void release( sb::Collector& collector,
              sb::Calculator& calculator,
              sb::Analyzer& analyzer )
{
	calculator.release();

	collector.release();

	analyzer.release();
}
