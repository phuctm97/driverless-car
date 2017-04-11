#include "Formatter.h"

sb::Formatter::Formatter( const cv::Rect& cropBox,
                          const cv::Point2f* warpOriginalSourceQuad,
                          const cv::Point2f* warpOriginalDestinationQuad,
                          double convertCoordCoef )
	: _cropBox( cropBox ), _convertCoordCoef( convertCoordCoef )
{
	cv::Point2f warpSourceQuad[4];
	cv::Point2f warpDestinationQuad[4];

	for ( int i = 0; i < 4; i++ ) {
		warpSourceQuad[i] = warpOriginalSourceQuad[i] - cv::Point2f( _cropBox.tl() );
		warpDestinationQuad[i] = warpOriginalDestinationQuad[i] - cv::Point2f( _cropBox.tl() );
	}

	_warpMatrix = cv::getPerspectiveTransform( warpSourceQuad, warpDestinationQuad );
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

int sb::Formatter::warp( const std::vector<sb::LineInfo*> imageLines,
                         std::vector<sb::LineInfo*>& outputRealLines ) const
{
	const int N_LINES = static_cast<int>(imageLines.size());

	if ( N_LINES == 0 )return 0;

	///// warp /////

	std::vector<cv::Point2f> startingPoints( N_LINES );
	std::vector<cv::Point2f> endingPoints( N_LINES );

	for ( int i = 0; i < N_LINES; i++ ) {
		startingPoints[i] = imageLines[i]->line.getStartingPoint();
		endingPoints[i] = imageLines[i]->line.getEndingPoint();
	}

	cv::perspectiveTransform( startingPoints, startingPoints, _warpMatrix );

	cv::perspectiveTransform( endingPoints, endingPoints, _warpMatrix );

	///// push to output array /////

	outputRealLines.reserve( N_LINES );

	for ( int i = 0; i < N_LINES; i++ ) {
		sb::LineInfo* lineInfo = new sb::LineInfo;
		sb::create( lineInfo, sb::Line( convertToCoord( endingPoints[i] ),
																		convertToCoord( startingPoints[i] ) ) );
		outputRealLines.push_back( lineInfo );
	}

	return 0;
}

double sb::Formatter::convertXToCoord( double x ) const
{
	return (x - _cropBox.width * 0.5) * _convertCoordCoef;
}

double sb::Formatter::convertYToCoord( double y ) const
{
	return (_cropBox.height - y) * _convertCoordCoef;
}

double sb::Formatter::convertLengthToCoord( double length ) const
{
	return length * _convertCoordCoef;
}

double sb::Formatter::convertLengthFromCoord( double length ) const
{
	return length / _convertCoordCoef;
}

cv::Point2d sb::Formatter::convertToCoord( const cv::Point2d& point ) const
{
	return cv::Point2d(
	                   convertXToCoord( point.x ),
	                   convertYToCoord( point.y )
	                  );
}

cv::Rect2d sb::Formatter::convertToCoord( const cv::Rect2d& rect ) const
{
	return cv::Rect2d(
	                  convertXToCoord( rect.x ),
	                  convertYToCoord( rect.y ),
	                  rect.width * _convertCoordCoef,
	                  rect.height * _convertCoordCoef
	                 );
}

double sb::Formatter::convertXFromCoord( double x ) const
{
	return (x / _convertCoordCoef) + (_cropBox.width * 0.5);
}

double sb::Formatter::convertYFromCoord( double y ) const
{
	return _cropBox.height - (y / _convertCoordCoef);
}

cv::Point2d sb::Formatter::convertFromCoord( const cv::Point2d& point ) const
{
	return cv::Point2d(
	                   convertXFromCoord( point.x ),
	                   convertYFromCoord( point.y )
	                  );
}

cv::Rect2d sb::Formatter::convertFromCoord( const cv::Rect2d& rect ) const
{
	return cv::Rect2d(
	                  convertXFromCoord( rect.x ),
	                  convertYFromCoord( rect.y ),
	                  rect.width / _convertCoordCoef,
	                  rect.height / _convertCoordCoef
	                 );
}
