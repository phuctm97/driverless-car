#include "../Classes/calculator/Calculator.h"
#include "../Classes/collector/Collector.h"

int init( sb::Collector& collector,
          sb::Calculator& calculator,
          const sb::Params& params );

void test( const sb::RawContent& rawContent,
           const sb::FrameInfo& frameInfo );

void release( sb::Collector& collector,
              sb::Calculator& calculator );

int main()
{
	sb::Params params;
	params.load( PARAMS_PATH );

	sb::RawContent rawContent;
	sb::FrameInfo frameInfo;

	sb::Collector collector;
	sb::Calculator calculator;

	if ( init( collector, calculator, params ) < 0 ) {
		std::cerr << "Init failed." << std::endl;
		return -1;
	}

	while ( true ) {

		if ( collector.collect( rawContent ) < 0 ) {
			std::cerr << "Collector collect failed." << std::endl;
			release( collector, calculator );
			return -1;
		}

		if ( calculator.calculate( rawContent, frameInfo ) < 0 ) {
			std::cerr << "Calculator calculate failed." << std::endl;
			release( collector, calculator );
			return -1;
		}

		test( rawContent, frameInfo );

		if( cv::waitKey( 33 ) == KEY_TO_ESCAPE ) break;
	}

	release( collector, calculator );
	return 0;
}

int init( sb::Collector& collector,
          sb::Calculator& calculator,
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

	return 0;
}

void test( const sb::RawContent& rawContent,
           const sb::FrameInfo& frameInfo )
{
	int expandHeight = 200;
	cv::Mat originalImage( frameInfo.getColorImage().rows + expandHeight, frameInfo.getColorImage().cols, CV_8UC3 );
	cv::Mat warpedImage( frameInfo.getColorImage().rows + expandHeight, frameInfo.getColorImage().cols, CV_8UC3 );

	for ( const auto& line : frameInfo.getLines() ) {
		cv::line(
			originalImage,
			line.getStartingPoint() + cv::Point2d( 0, expandHeight ),
			line.getEndingPoint() + cv::Point2d( 0, expandHeight ),
			cv::Scalar( 0, 255, 0 ), 1
		);
	}

	for( const auto& line : frameInfo.getWarpedLines() ) {
		cv::line(
			warpedImage,
			line.getStartingPoint() + cv::Point2d( 0, expandHeight ),
			line.getEndingPoint() + cv::Point2d( 0, expandHeight ),
			cv::Scalar( 0, 255, 255 ), 1
		);
	}

	cv::imshow( "Original", originalImage );
	cv::imshow( "Warped", warpedImage );

	cv::waitKey();
}

void release( sb::Collector& collector,
              sb::Calculator& calculator )
{
	calculator.release();

	collector.release();
}
