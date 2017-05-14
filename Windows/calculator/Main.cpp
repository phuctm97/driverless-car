#include "../Classes/calculator/Calculator.h"
#include "../Classes/collector/Collector.h"
#include "../Classes/Timer.h"
#include <conio.h>

int init( sb::Collector* collector,
          sb::Calculator* calculator,
          sb::Params* params );

<<<<<<< HEAD
void test( sb::Collector* collector,
           sb::Calculator* calculator,
           sb::RawContent* rawContent,
           sb::FrameInfo* frameInfo );

=======
void test( const sb::Collector& collector,
           const sb::Calculator& calculator,
           const sb::RawContent& rawContent,
           const sb::FrameInfo& frameInfo );

void release( sb::Collector& collector,
              sb::Calculator& calculator );

>>>>>>> master
int main( const int argc, const char** argv )
{
	if ( argc < 2 ) {
		std::cerr << "Can't find argument for Params path" << std::endl;
		return -1;
	}

<<<<<<< HEAD
	std::srand( static_cast<unsigned int>(time( 0 )) );

	// Application parameters
	sb::Params* params = new sb::Params;
	sb::load( params, argv[1] );
=======
	// Application parameters
	sb::Params params;
	params.load( argv[1] );

	// Timer for performance test
	sb::Timer timer;

	// Data sent&receive between components
	sb::RawContent rawContent;
	rawContent.create( params );
>>>>>>> master

	// Timer for performance test
	sb::Timer timer;

<<<<<<< HEAD
	// Data sent&receive between components
	sb::RawContent* rawContent = new sb::RawContent;
	sb::create( rawContent, params );

	sb::FrameInfo* frameInfo = new sb::FrameInfo;
	sb::create( frameInfo, params );

	// Main components
	sb::Collector* collector = new sb::Collector;

	sb::Calculator* calculator = new sb::Calculator;

=======
	// Main components
	sb::Collector collector;
	sb::Calculator calculator;

>>>>>>> master
	// Init components
	if ( init( collector, calculator, params ) < 0 ) {
		std::cerr << "Init failed." << std::endl;
		return -1;
	}

	// Pressed key
	char key = 0;
	std::cout << "Enter 's' to start! ";
<<<<<<< HEAD
	while ( key != 's' ) std::cin >> key;
=======
	while( key != 's' ) std::cin >> key;
>>>>>>> master

	// Timer
	int timerTickCount = 0;
	timer.reset( "entire-job" );

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
<<<<<<< HEAD

		////// </Collector> /////

		////// <Calculator> /////

		timer.reset( "calculator" );
		if ( sb::calculate( calculator, rawContent, frameInfo ) < 0 ) {
=======

		////// </Collector> /////


		////// <Calculator> /////

		timer.reset( "calculator" );
		if ( calculator.calculate( rawContent, frameInfo ) < 0 ) {
>>>>>>> master
			std::cerr << "Calculator calculate failed." << std::endl;
			break;
		}
		std::cout << "Calculator: " << timer.milliseconds( "calculator" ) << "ms." << std::endl;
<<<<<<< HEAD

		////// </Calculator> /////

=======

		////// </Calculator> /////

>>>>>>> master
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
<<<<<<< HEAD
	sb::release( params );
	delete params;

	sb::release( rawContent );
	delete rawContent;

	sb::release( frameInfo );
	delete frameInfo;

	sb::release( collector );
	delete collector;

	sb::release( calculator );
	delete calculator;

=======
	release( collector, calculator );
>>>>>>> master
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

<<<<<<< HEAD
void test( sb::Collector* collector,
           sb::Calculator* calculator,
           sb::RawContent* rawContent,
           sb::FrameInfo* frameInfo )
{
	cv::Mat debugImage = frameInfo->bgrImage;
	cv::imshow( "Calculator", debugImage );
	cv::waitKey();

	cv::imshow( "Calculator", frameInfo->binImage );
	cv::waitKey();
=======
void test( const sb::Collector& collector,
           const sb::Calculator& calculator,
           const sb::RawContent& rawContent,
           const sb::FrameInfo& frameInfo )
{
	const double FRAME_HALF_WIDTH = frameInfo.getColorImage().cols / 2;
	const int EXPAND_HEIGHT = 900;
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

	// debug each section
	for ( int i = 0; i < static_cast<int>(frameInfo.getSectionInfos().size()); i++ ) {
		const sb::SectionInfo& sectionInfo = frameInfo.getSectionInfos()[i];

		// debug each line in section
		for ( int j = 0; j < static_cast<int>(sectionInfo.lines.size()); j++ ) {
			const std::pair<int, cv::Vec2d> sectionLine = sectionInfo.lines[j];

			const sb::LineInfo& line = frameInfo.getImageLineInfos()[sectionLine.first];
			const sb::LineInfo& realLine = frameInfo.getRealLineInfos()[sectionLine.first];

			cv::Mat originalImage = frameInfo.getColorImage().clone();
			cv::Mat tempImage = realImage.clone();

			cv::line( originalImage,
			          line.getStartingPoint(),
			          line.getEndingPoint(),
			          cv::Scalar( 0, 255, 0 ), 2 );
			cv::imshow( "Original Image", originalImage );

			cv::line( tempImage,
			          cv::Point2d( 0, calculator.convertYFromCoord( sectionInfo.lowerRow ) ) + cv::Point2d( 0, EXPAND_HEIGHT ),
			          cv::Point2d( tempImage.cols - 1, calculator.convertYFromCoord( sectionInfo.lowerRow ) ) + cv::Point2d( EXPAND_WIDTH / 2, EXPAND_HEIGHT ),
			          cv::Scalar( 255, 255, 255 ), 1 );
			cv::line( tempImage,
			          cv::Point2d( 0, calculator.convertYFromCoord( sectionInfo.upperRow ) ) + cv::Point2d( 0, EXPAND_HEIGHT ),
			          cv::Point2d( tempImage.cols - 1, calculator.convertYFromCoord( sectionInfo.upperRow ) ) + cv::Point2d( EXPAND_WIDTH / 2, EXPAND_HEIGHT ),
			          cv::Scalar( 255, 255, 255 ), 1 );

			cv::line( tempImage,
			          calculator.convertFromCoord( realLine.getStartingPoint() )
			          + cv::Point2d( EXPAND_WIDTH / 2, EXPAND_HEIGHT ),
			          calculator.convertFromCoord( realLine.getEndingPoint() )
			          + cv::Point2d( EXPAND_WIDTH / 2, EXPAND_HEIGHT ),
			          cv::Scalar( 0, 255, 0 ), 2 );

			std::stringstream stringBuilder;

			double angle = realLine.getAngle();
			double lowerX = sectionLine.second[0];
			double upperX = sectionLine.second[1];

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

			stringBuilder << "Upper X: " << upperX;
			cv::putText( tempImage,
			             stringBuilder.str(),
			             cv::Point( 20, 55 ),
			             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

			stringBuilder.str( "" );

			stringBuilder << "Lower X: " << lowerX;
			cv::putText( tempImage,
			             stringBuilder.str(),
			             cv::Point( 20, 75 ),
			             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

			stringBuilder.str( "" );

			stringBuilder << "Color: " << realLine.getAverageColor();
			cv::putText( tempImage,
			             stringBuilder.str(),
			             cv::Point( 20, 95 ),
			             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

			cv::imshow( "Window", tempImage );
			cv::waitKey();
		}
	}
}
>>>>>>> master

	for ( auto cit_blob = frameInfo->blobs.cbegin(); cit_blob != frameInfo->blobs.cend(); ++cit_blob ) {
		cv::Mat img = debugImage.clone();

		sb::Blob* blob = *cit_blob;
		for ( auto cit_childblob = blob->childBlobs.cbegin(); cit_childblob != blob->childBlobs.cend(); ++cit_childblob ) {
			sb::Blob* childBlob = *cit_childblob;

			if ( childBlob->size == 0 ) continue;

			cv::rectangle( img, childBlob->box.tl(), childBlob->box.br(), cv::Scalar( 0, 0, 255 ), 1 );
			cv::circle( img, childBlob->origin, 3, cv::Scalar( 0, 255, 0 ), 2 );
		}

		cv::putText( img, std::to_string( blob->size ), cv::Point( frameInfo->bgrImage.cols / 2, 30 ),
		             cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar( 0, 255, 255 ), 2 );
		cv::imshow( "Calculator", img );
		cv::waitKey();
	}
}
