#include "../Classes/calculator/Calculator.h"
#include "../Classes/collector/Collector.h"
#include "../Classes/Timer.h"
#include <conio.h>

int init( sb::Collector& collector,
          sb::Calculator& calculator,
          const sb::Params& params );

void test( const sb::Collector& collector,
           const sb::Calculator& calculator,
           const sb::RawContent& rawContent,
           const sb::FrameInfo& frameInfo );

void release( sb::Collector& collector,
              sb::Calculator& calculator );

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

	// Main components
	sb::Collector collector;
	sb::Calculator calculator;

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

void test( const sb::Collector& collector,
           const sb::Calculator& calculator,
           const sb::RawContent& rawContent,
           const sb::FrameInfo& frameInfo )
{
	const double FRAME_HALF_WIDTH = frameInfo.getColorImage().cols / 2;
	const int EXPAND_HEIGHT = 720;
	const int EXPAND_WIDTH = 1200;

	// create real image
	cv::Mat realImage = cv::Mat::zeros( frameInfo.getColorImage().rows + EXPAND_HEIGHT,
	                                    frameInfo.getColorImage().cols + EXPAND_WIDTH,
	                                    CV_8UC3 );

	cv::line( realImage,
	          cv::Point2d( realImage.cols / 2, 0 ),
	          cv::Point2d( realImage.cols / 2, realImage.rows ),
	          cv::Scalar( 170, 170, 170 ) );
	for ( const auto& realLine : frameInfo.getRealLineInfos() ) {
		cv::line(
		         realImage,
		         calculator.convertFromCoord( realLine.getStartingPoint() )
		         + cv::Point2d( EXPAND_WIDTH / 2, EXPAND_HEIGHT ),
		         calculator.convertFromCoord( realLine.getEndingPoint() )
		         + cv::Point2d( EXPAND_WIDTH / 2, EXPAND_HEIGHT ),
		         cv::Scalar( 0, 0, 255 ), 1
		        );
	}

	cv::destroyWindow( "Window" );

	// debug each line in section
	for ( int j = 0; j < static_cast<int>(frameInfo.getRealLineInfos().size()); j++ ) {
		const sb::LineInfo& line = frameInfo.getImageLineInfos()[j];
		const sb::LineInfo& realLine = frameInfo.getRealLineInfos()[j];

		cv::Mat originalImage = frameInfo.getColorImage().clone();
		cv::Mat tempImage = realImage.clone();

		cv::line( originalImage,
		          line.getStartingPoint(),
		          line.getEndingPoint(),
		          cv::Scalar( 0, 255, 0 ), 2 );
		cv::imshow( "Original Image", originalImage );

		cv::line( tempImage,
		          calculator.convertFromCoord( realLine.getStartingPoint() )
		          + cv::Point2d( EXPAND_WIDTH / 2, EXPAND_HEIGHT ),
		          calculator.convertFromCoord( realLine.getEndingPoint() )
		          + cv::Point2d( EXPAND_WIDTH / 2, EXPAND_HEIGHT ),
		          cv::Scalar( 0, 255, 0 ), 2 );

		std::stringstream stringBuilder;

		double angle = realLine.getAngle();

		stringBuilder << "Length: " << realLine.getLength();
		cv::putText( tempImage,
		             stringBuilder.str(),
		             cv::Point( 20, 15 ),
		             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

		stringBuilder.str( "" );

		stringBuilder << "Angle: " << angle;
		cv::putText( tempImage,
		             stringBuilder.str(),
		             cv::Point( 20, 35 ),
		             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );
		stringBuilder.str( "" );

		stringBuilder << "Color: " << realLine.getAverageColor();
		cv::putText( tempImage,
		             stringBuilder.str(),
		             cv::Point( 20, 55 ),
		             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );
		stringBuilder.str( "" );

		cv::imshow( "Window", tempImage );
		cv::waitKey();
	}
}

void release( sb::Collector& collector,
              sb::Calculator& calculator )
{
	calculator.release();

	collector.release();
}
