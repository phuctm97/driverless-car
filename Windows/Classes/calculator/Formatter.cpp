#include "Formatter.h"

sb::Formatter::Formatter( const cv::Rect& cropBox,
                          const std::vector<int>& separateRows,
                          double convertCoordCoef,
                          const cv::Point2f* warpOriginalSourceQuad,
                          const cv::Point2f* warpOriginalDestinationQuad )
	: _cropBox( cropBox ), _separateRows( separateRows ), _convertCoordCoef( convertCoordCoef )
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
                         std::vector<sb::LineInfo>& outputLines,
                         cv::Point2d& topLeftPoint ) const
{
	outputLines.clear();

	const int N_LINES = static_cast<int>(originalLines.size());

	if ( N_LINES == 0 )return 0;

	///// warp /////

	cv::Matx33f warpMatrix = cv::getPerspectiveTransform( _warpSourceQuad, _warpDestinationQuad );

	std::vector<cv::Point2f> startingPoints( N_LINES );
	std::vector<cv::Point2f> endingPoints( N_LINES );

	for ( int i = 0; i < N_LINES; i++ ) {
		startingPoints[i] = originalLines[i].getStartingPoint();
		endingPoints[i] = originalLines[i].getEndingPoint();
	}

	cv::perspectiveTransform( startingPoints, startingPoints, warpMatrix );

	cv::perspectiveTransform( endingPoints, endingPoints, warpMatrix );

	///// push to output array /////

	outputLines.reserve( N_LINES );

	for ( int i = 0; i < N_LINES; i++ ) {
		// find top left point
		topLeftPoint.x = MIN( topLeftPoint.x, startingPoints[i].x );
		topLeftPoint.x = MIN( topLeftPoint.x, endingPoints[i].x );
		topLeftPoint.y = MIN( topLeftPoint.y, startingPoints[i].y );
		topLeftPoint.y = MIN( topLeftPoint.y, endingPoints[i].y );

		outputLines.push_back( sb::LineInfo( sb::Line( startingPoints[i], endingPoints[i] ) ) );
	}

	return 0;
}

int sb::Formatter::split( const std::vector<sb::LineInfo> warpedLines,
                          int containerHeight,
                          std::vector<sb::SectionInfo>& outputSections ) const
{
	outputSections.clear();

	const int N_SECTIONS = static_cast<int>(_separateRows.size()) - 1;
	const int N_LINES = static_cast<int>(warpedLines.size());

	if ( N_SECTIONS <= 0 )return -1;

	///// calculate section borders /////

	outputSections.assign( N_SECTIONS, sb::SectionInfo() );

	for ( int i = 0; i < N_SECTIONS; i++ ) {
		sb::SectionInfo& sectionInfo = outputSections[i];

		// borders
		sectionInfo.lowerRow = containerHeight - _separateRows[i];
		sectionInfo.upperRow = containerHeight - _separateRows[i + 1];

		// lines in section
		const sb::Line upperLine( cv::Point2d( 0, sectionInfo.upperRow ), cv::Point2d( 1, sectionInfo.upperRow ) );
		const sb::Line lowerLine( cv::Point2d( 0, sectionInfo.lowerRow ), cv::Point2d( 1, sectionInfo.lowerRow ) );

		sectionInfo.lines.clear();
		for ( int index = 0; index < N_LINES; index++ ) {
			const sb::LineInfo& lineInfo = warpedLines[index];

			if ( lineInfo.getStartingPoint().y <= sectionInfo.lowerRow &&
				lineInfo.getEndingPoint().y >= sectionInfo.upperRow ) {
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
	                   convertYFromCoord( point.x )
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
