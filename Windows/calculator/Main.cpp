#include "../Classes/calculator/Calculator.h"
#include "../Classes/collector/Collector.h"
#include "../Classes/Timer.h"
#include <conio.h>

int init( sb::Collector* collector,
          sb::Calculator* calculator,
          sb::Params* params );

void test( sb::Collector* collector,
           sb::Calculator* calculator,
           sb::RawContent* rawContent,
           sb::FrameInfo* frameInfo );

void release( sb::Collector* collector,
              sb::Calculator* calculator );

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

	// Main components
	sb::Collector* collector = new sb::Collector;

	sb::Calculator* calculator = new sb::Calculator;

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

	delete params;
	delete rawContent;

	sb::clear( frameInfo );
	delete frameInfo;

	delete collector;
	delete calculator;

	return 0;
}

int init( sb::Collector* collector,
          sb::Calculator* calculator,
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

	return 0;
}

void test( sb::Collector* collector,
           sb::Calculator* calculator,
           sb::RawContent* rawContent,
           sb::FrameInfo* frameInfo )
{
	// debug each line in section
	cv::Mat images[3];

	// edges image
	images[0] = frameInfo->edgesImage.clone();
	cv::cvtColor( images[0], images[0], cv::COLOR_GRAY2BGR );

	// lines image
	images[1] = cv::Mat( frameInfo->colorImage.size(), CV_8UC3, cv::Scalar( 0, 0, 0 ) );
	for ( auto it_section = frameInfo->imageSections.cbegin();
	      it_section != frameInfo->imageSections.cend(); ++it_section ) {

		for ( auto it_line = (*it_section)->imageLines.begin();
		      it_line != (*it_section)->imageLines.end(); ++it_line ) {
			cv::line( images[1],
			          (*it_line)->line.getStartingPoint(),
			          (*it_line)->line.getEndingPoint(),
			          cv::Scalar( 255, 255, 255 ) );
		}
	}

	// color image
	images[2] = frameInfo->colorImage.clone();

	// debug
	for ( int i = 0; i < 3; i++ ) {
		for ( auto it_section = frameInfo->imageSections.cbegin();
		      it_section != frameInfo->imageSections.cend(); ++it_section ) {
			cv::line( images[i],
			          cv::Point2d( 0, (*it_section)->topLine.getStartingPoint().y ),
			          cv::Point2d( images[i].cols - 1, (*it_section)->topLine.getStartingPoint().y ),
			          cv::Scalar( 255, 0, 0 ) );
			cv::line( images[i],
			          cv::Point2d( 0, (*it_section)->bottomLine.getStartingPoint().y ),
			          cv::Point2d( images[i].cols - 1, (*it_section)->bottomLine.getStartingPoint().y ),
			          cv::Scalar( 255, 0, 0 ) );
		}
	}

	for ( auto it_section = frameInfo->imageSections.cbegin();
	      it_section != frameInfo->imageSections.cend(); ++it_section ) {

		for ( auto it_line = (*it_section)->imageLines.cbegin();
		      it_line != (*it_section)->imageLines.cend(); ++it_line ) {

			std::stringstream stringBuilder;

			for ( int i = 0; i < 3; i++ ) {
				cv::Mat tmpImage = images[i].clone();

				cv::line( tmpImage,
				          (*it_line)->topPoint,
				          (*it_line)->bottomPoint,
				          cv::Scalar( 0, 255, 0 ), 2 );

				stringBuilder << "Length: " << (*it_line)->length;
				cv::putText( tmpImage,
				             stringBuilder.str(),
				             cv::Point( 20, 15 ),
				             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

				stringBuilder.str( "" );

				stringBuilder << "Angle: " << (*it_line)->angle;
				cv::putText( tmpImage,
				             stringBuilder.str(),
				             cv::Point( 200, 15 ),
				             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );
				stringBuilder.str( "" );

				stringBuilder << "Bottom X: " << (*it_line)->bottomPoint.x;
				cv::putText( tmpImage,
				             stringBuilder.str(),
				             cv::Point( 20, 35 ),
				             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );
				stringBuilder.str( "" );

				stringBuilder << "Top X: " << (*it_line)->topPoint.x;
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

void release( sb::Collector* collector,
              sb::Calculator* calculator )
{
	sb::release( calculator );

	sb::release( collector );
}
