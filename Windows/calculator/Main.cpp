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
	const int EXPAND_HEIGHT = static_cast<int>(abs( frameInfo.getTopLeftPoint().y ));
	const int EXPAND_WIDTH = static_cast<int>(abs( frameInfo.getTopLeftPoint().x )) * 2;

	// create original image
	cv::imshow( WINDOW_NAME, frameInfo.getColorImage() );
	cv::waitKey();

	// create warped image
	cv::Mat warpedImage = cv::Mat::zeros( frameInfo.getColorImage().rows + EXPAND_HEIGHT,
	                                      frameInfo.getColorImage().cols + EXPAND_WIDTH,
	                                      CV_8UC3 );

	for ( const auto& warpedLine : frameInfo.getWarpedLines() ) {
		cv::line(
		         warpedImage,
		         warpedLine.getStartingPoint() + cv::Point2d( EXPAND_WIDTH / 2, EXPAND_HEIGHT ),
		         warpedLine.getEndingPoint() + cv::Point2d( EXPAND_WIDTH / 2, EXPAND_HEIGHT ),
		         cv::Scalar( 0, 0, 255 ), 1
		        );
	}

	cv::destroyWindow( WINDOW_NAME );

	// debug each section
	for ( int i = 0; i < static_cast<int>(frameInfo.getSections().size()); i++ ) {
		const sb::SectionInfo& sectionInfo = frameInfo.getSections()[i];

		// debug each line in section
		for ( int j = 0; j < static_cast<int>(sectionInfo.lines.size()); j++ ) {
			const std::pair<int, cv::Vec2d> sectionLine = sectionInfo.lines[j];

			const sb::LineInfo& line = frameInfo.getLines()[sectionLine.first];

			const sb::LineInfo& warpedLine = frameInfo.getWarpedLines()[sectionLine.first];
			
			cv::Mat tempImage = warpedImage.clone();

			cv::line( tempImage,
								cv::Point2d( 0, sectionInfo.lowerRow ) + cv::Point2d( 0, EXPAND_HEIGHT ),
								cv::Point2d( tempImage.cols - 1, sectionInfo.lowerRow ) + cv::Point2d( EXPAND_WIDTH / 2, EXPAND_HEIGHT ),
								cv::Scalar( 255, 255, 255 ), 1 );
			cv::line( tempImage,
								cv::Point2d( 0, sectionInfo.upperRow ) + cv::Point2d( 0, EXPAND_HEIGHT ),
								cv::Point2d( tempImage.cols - 1, sectionInfo.upperRow ) + cv::Point2d( EXPAND_WIDTH / 2, EXPAND_HEIGHT ),
								cv::Scalar( 255, 255, 255 ), 1 );
			cv::line( tempImage,
								warpedLine.getStartingPoint() + cv::Point2d( EXPAND_WIDTH / 2, EXPAND_HEIGHT ),
								warpedLine.getEndingPoint() + cv::Point2d( EXPAND_WIDTH / 2, EXPAND_HEIGHT ),
								cv::Scalar( 0, 255, 0 ), 2 );

			std::stringstream stringBuilder;

			double rotation = 90 - warpedLine.getAngle();
			double upperX = sectionLine.second[0] / FRAME_HALF_WIDTH - 1;
			double lowerX = sectionLine.second[1] / FRAME_HALF_WIDTH - 1;

			stringBuilder << "Length: " << warpedLine.getLength();
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

			stringBuilder << "Color: " << line.getAverageColor();
			cv::putText( tempImage,
									 stringBuilder.str(),
									 cv::Point( 20, 95 ),
									 cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

			cv::imshow( WINDOW_NAME, tempImage );
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
