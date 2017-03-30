#include "Formatter.h"

sb::Formatter::Formatter( const cv::Rect& cropBox,
                          const cv::Point2f* warpOriginalSourceQuad,
                          const cv::Point2f* warpOriginalDestinationQuad,
                          double convertCoordCoef,
                          const std::vector<int>& separateRows )
	: _cropBox( cropBox ), _convertCoordCoef( convertCoordCoef ), _separateRows( separateRows )
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

int sb::Formatter::warp( const std::vector<sb::LineInfo> imageLines,
                         std::vector<sb::LineInfo>& outputRealLines ) const
{
	outputRealLines.clear();

	const int N_LINES = static_cast<int>(imageLines.size());

	if ( N_LINES == 0 )return 0;

	///// warp /////

	std::vector<cv::Point2f> startingPoints( N_LINES );
	std::vector<cv::Point2f> endingPoints( N_LINES );

	for ( int i = 0; i < N_LINES; i++ ) {
		startingPoints[i] = imageLines[i].getStartingPoint();
		endingPoints[i] = imageLines[i].getEndingPoint();
	}

	cv::perspectiveTransform( startingPoints, startingPoints, _warpMatrix );

	cv::perspectiveTransform( endingPoints, endingPoints, _warpMatrix );

	///// push to output array /////

	outputRealLines.reserve( N_LINES );

	for ( int i = 0; i < N_LINES; i++ ) {
		outputRealLines.push_back( sb::LineInfo( sb::Line( convertToCoord( startingPoints[i] ),
		                                                   convertToCoord( endingPoints[i] ) ),
		                                         imageLines[i].getAverageColor() ) );
	}

	return 0;
}

int sb::Formatter::split( const std::vector<sb::LineInfo> realLines,
                          std::vector<sb::SectionInfo>& outputSections ) const
{
	outputSections.clear();

	const int N_SECTIONS = static_cast<int>(_separateRows.size()) - 1;
	const int N_LINES = static_cast<int>(realLines.size());

	if ( N_SECTIONS <= 0 )return -1;

	///// calculate section borders /////

	outputSections.assign( N_SECTIONS, sb::SectionInfo() );

	for ( int i = 0; i < N_SECTIONS; i++ ) {
		sb::SectionInfo& sectionInfo = outputSections[i];

		// borders
		sectionInfo.upperRow = _separateRows[i + 1];
		sectionInfo.lowerRow = _separateRows[i];

		// lines in section
		const sb::Line upperLine( cv::Point2d( 0, sectionInfo.upperRow ), cv::Point2d( 1, sectionInfo.upperRow ) );
		const sb::Line lowerLine( cv::Point2d( 0, sectionInfo.lowerRow ), cv::Point2d( 1, sectionInfo.lowerRow ) );

		sectionInfo.lines.clear();
		for ( int index = 0; index < N_LINES; index++ ) {
			const sb::LineInfo& lineInfo = realLines[index];

			if ( lineInfo.getEndingPoint().y >= sectionInfo.lowerRow &&
				lineInfo.getStartingPoint().y <= sectionInfo.upperRow ) {
				cv::Vec2d vec;

				cv::Point2d p;

				if ( !sb::Line::findIntersection( lineInfo.getLine(), upperLine, p ) ) continue;
				vec[0] = p.x;

				if ( !sb::Line::findIntersection( lineInfo.getLine(), lowerLine, p ) ) continue;
				vec[1] = p.x;

				sectionInfo.lines.push_back( std::pair<int, cv::Vec2d>( index, vec ) );
			}
		}
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

cv::Point2d sb::Formatter::convertToCoord( const cv::Point2d& point ) const
{
	return cv::Point2d(
	                   convertXToCoord( point.x ),
	                   convertYToCoord( point.y )
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

double sb::Formatter::convertToRotation( double angle ) const
{
	return 90 - angle;
}

double sb::Formatter::convertFromRotation( double rotation ) const
{
	return 90 - rotation;
}
