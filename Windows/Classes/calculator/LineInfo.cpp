#include "LineInfo.h"

sb::LineInfo::LineInfo( const sb::Line& line )
	: _line( line ),
	  _angle( line.getAngleWithOx() ),
	  _length( line.getLength() ),
	  _averageColor( 0, 0, 0 ) {}

sb::LineInfo::LineInfo( const cv::Mat& colorImage, const sb::Line& line )
	: _line( line ),
	  _angle( line.getAngleWithOx() ),
	  _length( line.getLength() )
{
	calculateAverageColor( colorImage );
}

const sb::Line& sb::LineInfo::getLine() const { return _line; }

const cv::Point2d& sb::LineInfo::getStartingPoint() const { return _line.getStartingPoint(); }

const cv::Point2d& sb::LineInfo::getEndingPoint() const { return _line.getEndingPoint(); }

double sb::LineInfo::getLength() const { return _length; }

double sb::LineInfo::getAngle() const { return _angle; }

const cv::Vec3b& sb::LineInfo::getAverageColor() const { return _averageColor; }

void sb::LineInfo::calculateAverageColor( const cv::Mat& colorImage )
{
	cv::Point2d unitVec = getEndingPoint() - getStartingPoint();
	unitVec = cv::Point2d( unitVec.x / _length,
	                       unitVec.y / _length );

	cv::Vec3d sum( 0, 0, 0 );
	int n = 0;

	for ( int unit = 0; unit < _length; unit++ ) {

		int c = static_cast<int>(getStartingPoint().x + unitVec.x * unit);
		int r = static_cast<int>(getStartingPoint().y + unitVec.y * unit);

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

	_averageColor = cv::Vec3b( static_cast<uchar>(sum[0] / n),
	                           static_cast<uchar>(sum[1] / n),
	                           static_cast<uchar>(sum[2] / n) );
}
