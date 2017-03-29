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

		if ( cv::waitKey( 33 ) == KEY_TO_ESCAPE ) break;
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
	const double FRAME_HALF_WIDTH = frameInfo.getColorImage().cols / 2;
	const int EXPAND_HEIGHT = 200;

	// create original image
	cv::Mat originalImage = cv::Mat::zeros( frameInfo.getColorImage().rows + EXPAND_HEIGHT,
	                                        frameInfo.getColorImage().cols,
	                                        CV_8UC3 );

	for ( const auto& line : frameInfo.getLines() ) {
		cv::line(
		         originalImage,
		         line.getStartingPoint() + cv::Point2d( 0, EXPAND_HEIGHT ),
		         line.getEndingPoint() + cv::Point2d( 0, EXPAND_HEIGHT ),
		         cv::Scalar( 0, 0, 255 ), 1
		        );
	}

	// debug each line
	for ( auto line : frameInfo.getLines() ) {
		cv::Mat tempImage = originalImage.clone();

		cv::line( tempImage,
		          line.getStartingPoint() + cv::Point2d( 0, EXPAND_HEIGHT ),
		          line.getEndingPoint() + cv::Point2d( 0, EXPAND_HEIGHT ),
		          cv::Scalar( 0, 255, 0 ), 2 );

		std::stringstream stringBuilder;

		stringBuilder << "Length: " << line.getLength();
		cv::putText( tempImage,
		             stringBuilder.str(),
		             cv::Point( 20, 15 ),
		             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

		stringBuilder.str( "" );

		stringBuilder << "Angle: " << line.getAngle();
		cv::putText( tempImage,
		             stringBuilder.str(),
		             cv::Point( 20, 35 ),
		             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

		cv::imshow( WINDOW_NAME, tempImage );
		cv::waitKey();
	}

	// create warped image
	cv::Mat warpedImage = cv::Mat::zeros( frameInfo.getColorImage().rows + EXPAND_HEIGHT,
	                                      frameInfo.getColorImage().cols,
	                                      CV_8UC3 );

	for ( const auto& line : frameInfo.getWarpedLines() ) {
		cv::line(
		         warpedImage,
		         line.getStartingPoint() + cv::Point2d( 0, EXPAND_HEIGHT ),
		         line.getEndingPoint() + cv::Point2d( 0, EXPAND_HEIGHT ),
		         cv::Scalar( 0, 0, 255 ), 1
		        );
	}

	cv::destroyWindow( WINDOW_NAME );

	// debug each line
	for ( auto line : frameInfo.getWarpedLines() ) {
		cv::Mat tempImage = warpedImage.clone();

		cv::line( tempImage,
		          line.getStartingPoint() + cv::Point2d( 0, EXPAND_HEIGHT ),
		          line.getEndingPoint() + cv::Point2d( 0, EXPAND_HEIGHT ),
		          cv::Scalar( 0, 255, 0 ), 2 );

		std::stringstream stringBuilder;

		double rotation = 90 - line.getAngle();
		double positionX = line.getEndingPoint().x / FRAME_HALF_WIDTH - 1;

		stringBuilder << "Length: " << line.getLength();
		cv::putText( tempImage,
		             stringBuilder.str(),
		             cv::Point( 20, 15 ),
		             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

		stringBuilder.str( "" );

		stringBuilder << "Rotation: " << rotation;
		cv::putText( tempImage,
		             stringBuilder.str(),
		             cv::Point( 20, 35 ),
		             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );
		
		stringBuilder.str( "" );

		stringBuilder << "Position X: " << positionX;
		cv::putText( tempImage,
								 stringBuilder.str(),
								 cv::Point( 20, 55 ),
								 cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

		cv::imshow( WINDOW_NAME, tempImage );
		cv::waitKey();
	}

	cv::waitKey();
}

void release( sb::Collector& collector,
              sb::Calculator& calculator )
{
	calculator.release();

	collector.release();
}
