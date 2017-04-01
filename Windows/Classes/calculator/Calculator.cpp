#include "Calculator.h"

int sb::Calculator::init( const sb::Params& params )
{
	cv::Point cropPosition;
	cropPosition.x = (params.COLOR_FRAME_SIZE.width - params.CROPPED_FRAME_SIZE.width) / 2;
	cropPosition.y = params.COLOR_FRAME_SIZE.height - params.CROPPED_FRAME_SIZE.height;

	_formatter = sb::Formatter( cv::Rect( cropPosition.x, cropPosition.y,
	                                      params.CROPPED_FRAME_SIZE.width, params.CROPPED_FRAME_SIZE.height ),
	                            params.WARP_SRC_QUAD,
	                            params.WARP_DST_QUAD,
	                            params.CONVERT_COORD_COEF,
	                            params.SEPERATE_ROWS );

	_edgeDetector = sb::EdgeDetector( params.EDGE_DETECTOR_KERNEL_SIZE,
	                                  params.EDGE_DETECTOR_LOW_THRESH,
	                                  params.EDGE_DETECTOR_HIGH_THRESH,
	                                  params.BINARIZE_THRESH,
	                                  params.BINARIZE_MAX_VALUE );

	_lineDetector = sb::LineDetector( params.HOUGH_LINES_P_RHO,
	                                  params.HOUGH_LINES_P_THETA,
	                                  params.HOUGH_LINES_P_THRESHOLD,
	                                  params.HOUGH_LINES_P_MIN_LINE_LENGTH,
	                                  params.HOUGH_LINES_P_MAX_LINE_GAP );

	return 0;
}

int sb::Calculator::calculate( const sb::RawContent& rawContent,
                               sb::FrameInfo& frameInfo ) const
{
	///// Color image /////
	cv::Mat colorImage;
	if ( _formatter.crop( rawContent.getColorImage(), colorImage ) < 0 ) {
		std::cerr << "Crop image failed." << std::endl;
		return -1;
	}

	// flip to natural direction ( input image from camera was flipped )
	cv::flip( colorImage, colorImage, 1 );

	frameInfo.setColorImage( colorImage );

	///// Lines //////
	std::vector<sb::LineInfo> imageLineInfos;
	std::vector<sb::LineInfo> realLineInfos;

	// 1) generate edges-frame
	cv::Mat edgesFrame;
	cv::cvtColor( colorImage, edgesFrame, cv::COLOR_BGR2GRAY );
	_edgeDetector.apply( edgesFrame );

	// 2) generate lines in whole frame
	std::vector<sb::Line> lines;
	_lineDetector.apply( edgesFrame, lines );
	calculateLineInfos( lines, colorImage, imageLineInfos );

	// 3) generate warped lines
	if ( _formatter.warp( imageLineInfos, realLineInfos ) < 0 ) {
		std::cerr << "Warp lines failed." << std::endl;
		return -1;
	}

	frameInfo.setImageLineInfos( imageLineInfos );
	frameInfo.setRealLineInfos( realLineInfos );

	///// Sections //////
	std::vector<sb::SectionInfo> sectionInfos;

	if ( _formatter.split( realLineInfos, sectionInfos ) < 0 ) {
		std::cerr << "Split sections failed." << std::endl;
		return -1;
	}

	frameInfo.setSectionInfos( sectionInfos );

	return 0;
}

void sb::Calculator::release() {}

double sb::Calculator::convertXToCoord( double x ) const { return _formatter.convertXToCoord( x ); }

double sb::Calculator::convertYToCoord( double y ) const { return _formatter.convertYToCoord( y ); }

cv::Point2d sb::Calculator::convertToCoord( const cv::Point2d& point ) const { return _formatter.convertToCoord( point ); }

double sb::Calculator::convertXFromCoord( double x ) const { return _formatter.convertXFromCoord( x ); }

double sb::Calculator::convertYFromCoord( double y ) const { return _formatter.convertYFromCoord( y ); }

cv::Point2d sb::Calculator::convertFromCoord( const cv::Point2d& point ) const { return _formatter.convertFromCoord( point ); }

void sb::Calculator::calculateLineInfos( const std::vector<sb::Line>& lines,
                                         const cv::Mat& colorImage,
                                         std::vector<sb::LineInfo>& outputLineInfos )
{
	outputLineInfos.clear();
	outputLineInfos.assign( lines.size(), sb::Line() );

	for ( size_t i = 0; i < lines.size(); i++ ) {
		const sb::Line& line = lines[i];
		sb::LineInfo& lineInfo = outputLineInfos[i];

		lineInfo.setLine( line );

		// unit vector for sweeping
		cv::Point2d unitVec = line.getEndingPoint() - line.getStartingPoint();
		unitVec = cv::Point2d( unitVec.x / lineInfo.getLength(),
		                       unitVec.y / lineInfo.getLength() );

		// sweep to summarize line color intensities
		cv::Vec3d sum( 0, 0, 0 );
		int n = 0;
		for ( int unit = 0; unit < lineInfo.getLength(); unit++ ) {

			int c = static_cast<int>(line.getStartingPoint().x + unitVec.x * unit);
			int r = static_cast<int>(line.getStartingPoint().y + unitVec.y * unit);

			sum += colorImage.at<cv::Vec3b>( r, c );
			n++;

			// sweep with window 3x3
			/*if( c > 0 ) {
			sum += colorImage.at<cv::Vec3b>( r, c - 1 ); n++;
			}
			if( r > 0 ) {
			sum += colorImage.at<cv::Vec3b>( r - 1, c ); n++;
			}
			if( c > 0 && r > 0 ) {
			sum += colorImage.at<cv::Vec3b>( r - 1, c - 1 ); n++;
			}
			if( c < colorImage.cols - 1 ) {
			sum += colorImage.at<cv::Vec3b>( r, c + 1 ); n++;
			}
			if( r < colorImage.rows - 1 ) {
			sum += colorImage.at<cv::Vec3b>( r + 1, c ); n++;
			}
			if( c < colorImage.cols - 1 && r < colorImage.rows - 1 ) {
			sum += colorImage.at<cv::Vec3b>( r + 1, c + 1 ); n++;
			}*/
		}

		cv::Vec3b averageColor = cv::Vec3b( static_cast<uchar>(sum[0] / n),
		                                    static_cast<uchar>(sum[1] / n),
		                                    static_cast<uchar>(sum[2] / n) );
		lineInfo.setAverageColor( averageColor );
	}
}
