#pragma region OpenCV References
// OpenCV References
#include <opencv2/opencv.hpp>
#pragma endregion

#pragma region STD References
// STD References
#include <ctime>
#pragma endregion

#pragma region SB References
// SB References
#include "Params.h"
#include "FrameInfo.h"
#include "Evaluator.h"
#pragma endregion

void parseCommands( const int argc, const char** argv,
										cv::String& VIDEO_PATH,
										cv::String& PARAMS_PATH );

void initImageStream( cv::VideoCapture& imageStream,
											const cv::String& VIDEO_PATH );

void initWindow( int width, int height );

bool loadFrameInfo( cv::VideoCapture& imageStream,
										sb::FrameInfo& frameInfo,
										const cv::Rect& areaToCrop,
										const sb::JoinerSplitter& joinerSplitter,
										const sb::EdgeDetector& edgeDetector,
										const sb::LineDetector& lineDetector );

void evaluateLines( sb::FrameInfo& frameInfo,
										const sb::Evaluator& evaluator );

void showColorFrame( const sb::FrameInfo& frameInfo );

void showLinesInSections( const sb::FrameInfo& frameInfo );

void showLinesInFrame( const sb::FrameInfo& frameInfo );

void debugLinesInSections( const sb::FrameInfo& frameInfo );

void debugLinesInFrame( const sb::FrameInfo& frameInfo );

void showResult();

int main( const int argc, const char* argv[] )
{
	// A.0) Program arguments
	cv::String VIDEO_PATH;
	cv::String PARAMS_PATH;
	parseCommands( argc, argv, VIDEO_PATH, PARAMS_PATH );

	// ---------------

	// A.a) Image stream, can be obtained from either video file (for test) or camera (for real-time working)
	cv::VideoCapture imageStream;
	initImageStream( imageStream, VIDEO_PATH );

	// A.b) Frame info
	sb::FrameInfo frameInfo;

	// ---------------

	// B) Algorithm parameters (static parameters)
	sb::Params params( PARAMS_PATH );

	// ---------------

	// C) Frame joiner-splitter
	sb::JoinerSplitter joinerSplitter( params.getRatioToSplit() );

	// D) Edge detector
	sb::EdgeDetector edgeDetector( params.getEdgeDetectorKernelSize(),
																 params.getEdgeDetectorLowThresh(),
																 params.getEdgeDetectorHighThresh(),
																 params.getEdgeDetectorBinarizeThresh(),
																 params.getEdgeDetectorBinarizeMaxValue() );

	// E) Line detector
	sb::LineDetector lineDetector( params.getHoughLinesPRho(),
																 params.getHoughLinesPTheta(),
																 params.getHoughLinesPThreshold(),
																 params.getHoughLinesPMinLineLength(),
																 params.getHoughLinesPMaxLineGap() );
	// D) Evaluator
	sb::Evaluator evaluator;

	// ---------------

	// 0) Initial Pause
	initWindow( static_cast<int>(imageStream.get( CV_CAP_PROP_FRAME_WIDTH )),
							static_cast<int>(imageStream.get( CV_CAP_PROP_FRAME_HEIGHT )) );

	while( true ) {

		// 1) load frame info
		if( !loadFrameInfo( imageStream,
												frameInfo,
												params.getAreaToCrop(), // * consider move to params
												joinerSplitter,
												edgeDetector,
												lineDetector ) )
			break;

		// 2) evaluate lines
		evaluateLines( frameInfo, evaluator );

		/*[DEBUG]*/
		showColorFrame( frameInfo );

		/*[DEBUG]*/
		debugLinesInFrame( frameInfo );

		/*[DEBUG]*/
		debugLinesInSections( frameInfo );

		if( cv::waitKey( 33 ) == KEY_TO_ESCAPE )break;
	}

	return 0;
}

void parseCommands( const int argc, const char** argv,
										cv::String& VIDEO_PATH,
										cv::String& PARAMS_PATH )
{
	cv::CommandLineParser parser( argc, argv,
																"{v           |" CAMERA_USE_MACRO " | video path        }"
																"{p           |<none>               | params path       }" );

	VIDEO_PATH = parser.get<cv::String>( "v" );
	PARAMS_PATH = parser.get<cv::String>( "p" );
}

void initImageStream( cv::VideoCapture& imageStream,
											const cv::String& VIDEO_PATH )
{
	if( VIDEO_PATH == CAMERA_USE_MACRO ) {
		imageStream = cv::VideoCapture( 0 );
	}
	else {
		imageStream = cv::VideoCapture( VIDEO_PATH );
	}
}

void initWindow( int width, int height )
{
	cv::resizeWindow( WINDOW_NAME, width, height );

	cv::Mat tempImage = cv::Mat::zeros( height, width, CV_8UC3 );

	cv::putText( tempImage,
							 "Press any key to start!",
							 cv::Point( 20, 70 ),
							 cv::FONT_HERSHEY_TRIPLEX, 1, cv::Scalar( 0, 255, 255 ), 1 );

	cv::imshow( WINDOW_NAME, tempImage );

	cv::waitKey();
}

bool loadFrameInfo( cv::VideoCapture& imageStream,
										sb::FrameInfo& frameInfo,
										const cv::Rect& areaToCrop,
										const sb::JoinerSplitter& joinerSplitter,
										const sb::EdgeDetector& edgeDetector,
										const sb::LineDetector& lineDetector )
{
	// 1) load frame from stream
	cv::Mat colorImage;
	imageStream >> colorImage;

	// 2) stream disconnected or wrong stream input, exit
	if( colorImage.empty() ) {
		std::cerr << "Stream disconnected!" << std::endl;
		return false;
	}
	else if( colorImage.channels() != 3 ) {
		std::cerr << "Wrong stream input!" << std::endl;
		return false;
	}

	// 3) crop
	cv::Mat colorFrame;
	if( areaToCrop.width > 0 && areaToCrop.height > 0 ) {
		colorFrame = colorImage( areaToCrop );
	}
	else {
		colorFrame = colorImage;
	}

	// 4) generate frame info
	frameInfo.create( colorFrame, joinerSplitter, edgeDetector, lineDetector );

	return true;
}

void evaluateLines( sb::FrameInfo& frameInfo, const sb::Evaluator& evaluator )
{
	for( sb::Line& line : frameInfo.getLines() ) {
		evaluator.staticEvaluate( line );
	}

	for( sb::Section& section : frameInfo.getSections() )
		for( sb::Line& line : section.getLines() ) {
			evaluator.staticEvaluate( line );
		}
}

void showColorFrame( const sb::FrameInfo& frameInfo )
{
	cv::imshow( WINDOW_NAME, frameInfo.getColorFrame() );
	cv::waitKey();
}

void showLinesInSections( const sb::FrameInfo& frameInfo )
{
	cv::Mat tempImage( frameInfo.getColorFrame().size(), CV_8UC3, cv::Scalar::all( 0 ) );

	for( auto section : frameInfo.getSections() ) {
		for( auto line : section.getLines() )
			cv::line( tempImage, line.getStartingPoint(), line.getEndingPoint(), cv::Scalar( 0, 255, 0 ) );
	}
	cv::imshow( WINDOW_NAME, tempImage );
	cv::waitKey();
}

void showLinesInFrame( const sb::FrameInfo& frameInfo )
{
	cv::Mat tempImage( frameInfo.getColorFrame().size(), CV_8UC3, cv::Scalar::all( 0 ) );

	for( auto line : frameInfo.getLines() ) {
		cv::line( tempImage, line.getStartingPoint(), line.getEndingPoint(), cv::Scalar( 0, 0, 255 ) );
	}

	cv::imshow( WINDOW_NAME, tempImage );
	cv::waitKey();
}

void debugLinesInFrame( const sb::FrameInfo& frameInfo )
{
	cv::Mat lineImage( frameInfo.getColorFrame().size(), CV_8UC3, cv::Scalar::all( 0 ) );

	for( auto line : frameInfo.getLines() ) {
		cv::line( lineImage, line.getStartingPoint(), line.getEndingPoint(), cv::Scalar( 0, 0, 255 ) );
	}

	for( auto line : frameInfo.getLines() ) {
		cv::Mat tempImage = lineImage.clone();

		cv::line( tempImage, line.getStartingPoint(), line.getEndingPoint(), cv::Scalar( 0, 255, 0 ), 2 );

		std::stringstream stringBuilder;

		stringBuilder << "Length: " << line.getLength();
		cv::putText( tempImage,
								 stringBuilder.str(),
								 cv::Point( 20, 15 ),
								 cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

		stringBuilder.str( "" );

		stringBuilder << "Angle: " << line.getAngleWithOx();
		cv::putText( tempImage,
								 stringBuilder.str(),
								 cv::Point( 20, 35 ),
								 cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

		stringBuilder.str( "" );

		stringBuilder << "Rate(Lane): " << line.getRating().rateToBeLane;
		cv::putText( tempImage,
								 stringBuilder.str(),
								 cv::Point( 20, 55 ),
								 cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

		stringBuilder.str( "" );

		stringBuilder << "Rate(Uneven): " << line.getRating().rateToBeUneven;
		cv::putText( tempImage,
								 stringBuilder.str(),
								 cv::Point( 20, 75 ),
								 cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

		stringBuilder.str( "" );

		stringBuilder << "Rate(Right): " << line.getRating().rateToBeRight;
		cv::putText( tempImage,
								 stringBuilder.str(),
								 cv::Point( 20, 95 ),
								 cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

		cv::imshow( WINDOW_NAME, tempImage );
		cv::waitKey();
	}
}

void debugLinesInSections( const sb::FrameInfo& frameInfo )
{
	cv::Mat lineImage( frameInfo.getColorFrame().size(), CV_8UC3, cv::Scalar::all( 0 ) );

	for( auto section : frameInfo.getSections() ) {
		for( auto line : section.getLines() ) {
			cv::line( lineImage, line.getStartingPoint(), line.getEndingPoint(), cv::Scalar( 0, 0, 255 ) );
		}
	}

	for( auto section : frameInfo.getSections() ) {
		for( auto line : section.getLines() ) {
			cv::Mat tempImage = lineImage.clone();

			cv::line( tempImage, line.getStartingPoint(), line.getEndingPoint(), cv::Scalar( 0, 255, 0 ), 2 );

			std::stringstream stringBuilder;

			stringBuilder << "Length: " << line.getLength();
			cv::putText( tempImage,
									 stringBuilder.str(),
									 cv::Point( 20, 15 ),
									 cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

			stringBuilder.str( "" );

			stringBuilder << "Angle: " << line.getAngleWithOx();
			cv::putText( tempImage,
									 stringBuilder.str(),
									 cv::Point( 20, 35 ),
									 cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

			stringBuilder.str( "" );

			stringBuilder << "Rate(Lane): " << line.getRating().rateToBeLane;
			cv::putText( tempImage,
									 stringBuilder.str(),
									 cv::Point( 20, 55 ),
									 cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

			stringBuilder.str( "" );

			stringBuilder << "Rate(Uneven): " << line.getRating().rateToBeUneven;
			cv::putText( tempImage,
									 stringBuilder.str(),
									 cv::Point( 20, 75 ),
									 cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

			stringBuilder.str( "" );

			stringBuilder << "Rate(Right): " << line.getRating().rateToBeRight;
			cv::putText( tempImage,
									 stringBuilder.str(),
									 cv::Point( 20, 95 ),
									 cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

			cv::imshow( WINDOW_NAME, tempImage );
			cv::waitKey();
		}
	}
}

void showResult() {}
