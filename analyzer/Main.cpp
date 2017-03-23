#include "../Classes/calculator/Calculator.h"
#include "../Classes/collector/Collector.h"
#include "../Classes/analyzer/Analyzer.h"

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

		if ( cv::waitKey( 33 ) == KEY_TO_ESCAPE ) break;
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
	const cv::Mat& originalImage = rawContent.getColorImage();

	const cv::Size cropRealSize(
	                            static_cast<int>(params.CROP_SIZE_WIDTH * originalImage.cols),
	                            static_cast<int>(params.CROP_SIZE_HEIGHT * originalImage.rows)
	                           );

	const cv::Point2f bias = cv::Point2f(
	                                     float( (originalImage.cols - cropRealSize.width) / 2 ),
	                                     float( originalImage.rows - cropRealSize.height )
	                                    );

	int expandHeight = 200;
	cv::Mat tempMat( frameInfo.getColorImage().rows + expandHeight, frameInfo.getColorImage().cols, CV_8UC3 );


	// copy lines from frame
	std::vector<sb::Line> lines;
	for ( const auto& line : frameInfo.getLines() ) {
		lines.push_back( sb::Line( line.getStartingPoint(),
		                           line.getEndingPoint() ) );
	}

	// show lines
	tempMat.setTo( cv::Scalar::all( 0 ) );
	for ( const auto& line : lines ) {
		cv::line(
		         tempMat,
		         line.getStartingPoint() + cv::Point2d( 0, expandHeight ),
		         line.getEndingPoint() + cv::Point2d( 0, expandHeight ),
		         cv::Scalar( 0, 255, 255 ), 1
		        );
	}

	cv::imshow( WINDOW_NAME, tempMat );
	cv::waitKey();

	// warp lines
	{
		cv::Point2f srcQuad[4];
		cv::Point2f dstQuad[4];
		for ( int i = 0; i < 4; i++ ) {
			srcQuad[i] = params.WARP_SRC_QUAD[i] - bias;
			dstQuad[i] = params.WARP_DST_QUAD[i] - bias;
		}

		cv::Matx33f warpMatrix = cv::getPerspectiveTransform( srcQuad, dstQuad );

		std::vector<cv::Point2f> startingPoints;
		std::vector<cv::Point2f> endingPoints;
		for ( const auto& line : lines ) {
			startingPoints.push_back( line.getStartingPoint() );
			endingPoints.push_back( line.getEndingPoint() );
		}

		cv::perspectiveTransform( startingPoints, startingPoints, warpMatrix );
		cv::perspectiveTransform( endingPoints, endingPoints, warpMatrix );

		for ( int i = 0; i < static_cast<int>(lines.size()); i++ ) {
			lines[i] = sb::Line( startingPoints[i], endingPoints[i] );
		}
	}

	// show lines
	tempMat.setTo( cv::Scalar::all( 0 ) );
	for ( const auto& line : lines ) {
		cv::line(
		         tempMat,
		         line.getStartingPoint() + cv::Point2d( 0, expandHeight ),
		         line.getEndingPoint() + cv::Point2d( 0, expandHeight ),
		         cv::Scalar( 0, 255, 255 ), 1
		        );
	}

	cv::imshow( WINDOW_NAME, tempMat );
	cv::waitKey();
}

void release( sb::Collector& collector,
              sb::Calculator& calculator,
              sb::Analyzer& analyzer )
{
	calculator.release();

	collector.release();

	analyzer.release();
}
