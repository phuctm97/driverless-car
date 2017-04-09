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
	// debug each line in section
	cv::Mat images[3];

	// edges image
	images[0] = frameInfo.getEdgesImage().clone();
	cv::cvtColor( images[0], images[0], cv::COLOR_GRAY2BGR );

	// lines image
	images[1] = cv::Mat( frameInfo.getColorImage().size(), CV_8UC3, cv::Scalar( 0, 0, 0 ) );
	for ( auto it_section = frameInfo.getImageSections().cbegin();
	      it_section != frameInfo.getImageSections().cend(); ++it_section ) {

		for ( auto it_line = it_section->getImageLines().begin();
		      it_line != it_section->getImageLines().end(); ++it_line ) {
			cv::line( images[1],
			          it_line->getStartingPoint(),
			          it_line->getEndingPoint(),
			          cv::Scalar( 255, 255, 255 ) );
		}
	}

	// color image
	images[2] = frameInfo.getColorImage().clone();

	// debug
	for ( int i = 0; i < 3; i++ ) {
		for ( auto it_section = frameInfo.getImageSections().cbegin();
		      it_section != frameInfo.getImageSections().cend(); ++it_section ) {
			cv::line( images[i],
			          cv::Point2d( 0, it_section->getTopLine().getStartingPoint().y ),
			          cv::Point2d( images[i].cols - 1, it_section->getTopLine().getStartingPoint().y ),
			          cv::Scalar( 255, 0, 0 ) );
			cv::line( images[i],
			          cv::Point2d( 0, it_section->getBottomLine().getStartingPoint().y ),
			          cv::Point2d( images[i].cols - 1, it_section->getBottomLine().getStartingPoint().y ),
			          cv::Scalar( 255, 0, 0 ) );
		}
	}

	for ( auto it_section = frameInfo.getImageSections().cbegin();
	      it_section != frameInfo.getImageSections().cend(); ++it_section ) {

		for ( auto it_line = it_section->getImageLines().cbegin();
		      it_line != it_section->getImageLines().cend(); ++it_line ) {

			std::stringstream stringBuilder;

			for ( int i = 0; i < 3; i++ ) {
				cv::Mat tmpImage = images[i].clone();

				cv::line( tmpImage,
				          it_line->getTopPoint(),
				          it_line->getBottomPoint(),
				          cv::Scalar( 0, 255, 0 ), 2 );

				stringBuilder << "Length: " << it_line->getLength();
				cv::putText( tmpImage,
				             stringBuilder.str(),
				             cv::Point( 20, 15 ),
				             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

				stringBuilder.str( "" );

				stringBuilder << "Angle: " << it_line->getAngle();
				cv::putText( tmpImage,
				             stringBuilder.str(),
				             cv::Point( 200, 15 ),
				             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );
				stringBuilder.str( "" );

				stringBuilder << "Bottom X: " << it_line->getBottomPoint().x;
				cv::putText( tmpImage,
				             stringBuilder.str(),
				             cv::Point( 20, 35 ),
				             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );
				stringBuilder.str( "" );

				stringBuilder << "Top X: " << it_line->getTopPoint().x;
				cv::putText( tmpImage,
				             stringBuilder.str(),
				             cv::Point( 200, 35 ),
				             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );
				stringBuilder.str( "" );

				stringBuilder << "Image " << i;
				cv::imshow( stringBuilder.str(), tmpImage );
				stringBuilder.str( "" );
			}

			cv::waitKey();

		}

	}
}

void release( sb::Collector& collector,
              sb::Calculator& calculator )
{
	calculator.release();

	collector.release();
}
