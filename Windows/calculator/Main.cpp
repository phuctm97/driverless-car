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
	rawContent.create( params );

	sb::FrameInfo frameInfo;
	frameInfo.create( params );

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
	const int EXPAND_HEIGHT = 700; // static_cast<int>(abs( frameInfo.getTopLeftPoint().y ));
	const int EXPAND_WIDTH = 900; // static_cast<int>(abs( frameInfo.getTopLeftPoint().x )) * 2;

	// create original image
	cv::imshow( "Original Image", frameInfo.getColorImage() );
	cv::waitKey();

	// create warped image
	cv::Mat warpedImage = cv::Mat::zeros( frameInfo.getColorImage().rows + EXPAND_HEIGHT,
	                                      frameInfo.getColorImage().cols + EXPAND_WIDTH,
	                                      CV_8UC3 );

	for ( const auto& warpedLine : frameInfo.getWarpedLineInfos() ) {
		cv::line(
		         warpedImage,
		         warpedLine.getStartingPoint() + cv::Point2d( EXPAND_WIDTH / 2, EXPAND_HEIGHT ),
		         warpedLine.getEndingPoint() + cv::Point2d( EXPAND_WIDTH / 2, EXPAND_HEIGHT ),
		         cv::Scalar( 0, 0, 255 ), 1
		        );
	}

	cv::destroyWindow( WINDOW_NAME );

	// debug each section
	for ( int i = 0; i < static_cast<int>(frameInfo.getSectionInfos().size()); i++ ) {
		const sb::SectionInfo& sectionInfo = frameInfo.getSectionInfos()[i];

		// debug each line in section
		for ( int j = 0; j < static_cast<int>(sectionInfo.lines.size()); j++ ) {
			const std::pair<int, cv::Vec2d> sectionLine = sectionInfo.lines[j];

			const sb::LineInfo& line = frameInfo.getLineInfos()[sectionLine.first];

			cv::Mat tempFrame = frameInfo.getColorImage().clone();

			cv::line( tempFrame,
								line.getStartingPoint(),
								line.getEndingPoint(),
								cv::Scalar( 0, 255, 0 ), 2 );

			cv::imshow( "Original Image", tempFrame );

			const sb::LineInfo& warpedLine = frameInfo.getWarpedLineInfos()[sectionLine.first];

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

			double rotation = /*frameInfo.convertToRotation*/( warpedLine.getAngle() );
			double upperX = /*frameInfo.convertXToCoord*/( sectionLine.second[0] );
			double lowerX = /*frameInfo.convertXToCoord*/( sectionLine.second[1] );

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
