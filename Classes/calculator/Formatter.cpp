#include "Formatter.h"

sb::Formatter::Formatter( const cv::Rect& cropBox,
                          const cv::Point2f* warpOriginalSourceQuad,
                          const cv::Point2f* warpOriginalDestinationQuad )
	: _cropBox( cropBox )
{
	for ( int i = 0; i < 4; i++ ) {
		_warpSourceQuad[i] = warpOriginalSourceQuad[i] - cv::Point2f( _cropBox.tl() );
		_warpDestinationQuad[i] = warpOriginalDestinationQuad[i] - cv::Point2f( _cropBox.tl() );
	}
}

int sb::Formatter::crop( const cv::Mat& inputImage, cv::Mat& outputImage ) const
{
	if ( _cropBox.x < 0 || _cropBox.y < 0 ||
		_cropBox.x + _cropBox.width > inputImage.cols ||
		_cropBox.y + _cropBox.height > inputImage.rows ) {
		std::cerr << "Input image and crop box aren't suitable." << std::endl;
		return -1;
	}

	outputImage = inputImage( _cropBox );

	return 0;
}

int sb::Formatter::warp( const std::vector<sb::LineInfo> originalLines,
                         std::vector<sb::LineInfo>& outputLines ) const
{
	const int N_LINES = static_cast<int>(originalLines.size());

	cv::Matx33f warpMatrix = cv::getPerspectiveTransform( _warpSourceQuad, _warpDestinationQuad );

	std::vector<cv::Point2f> startingPoints( N_LINES );
	std::vector<cv::Point2f> endingPoints( N_LINES );

	for ( int i = 0; i < N_LINES; i++ ) {
		startingPoints[i] = originalLines[i].getStartingPoint();
		endingPoints[i] = originalLines[i].getEndingPoint();
	}

	cv::perspectiveTransform( startingPoints, startingPoints, warpMatrix );

	cv::perspectiveTransform( endingPoints, endingPoints, warpMatrix );

	outputLines.clear();

	outputLines.reserve( N_LINES );

	for ( int i = 0; i < N_LINES; i++ ) {
		outputLines.push_back( sb::LineInfo( sb::Line( startingPoints[i], endingPoints[i] ) ) );
	}

	return 0;
}
