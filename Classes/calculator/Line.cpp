#include "Line.h"

sb::Line::Line( const cv::Point2d& point1,
                const cv::Point2d& point2 )
{
	if ( point1.y < point2.y ) {
		_startingPoint = point1;
		_endingPoint = point2;
	}
	else {
		_startingPoint = point2;
		_endingPoint = point1;
	}

	regenerateCoefs();
}

sb::Line::Line( double alpha, const cv::Point2d& p, bool degree )
	: _endingPoint( 0, 0 )
{
	if ( !degree ) alpha = alpha * 360 / CV_2PI;

	if ( alpha == 90 ) {
		_a = 1;
		_b = 0;

		_startingPoint = p;
		_c = -(_a * _startingPoint.x + _b * _startingPoint.y);
	}
	else {
		_a = tan( -alpha * CV_2PI / 360 );
		_b = -1;

		_startingPoint = p;
		_c = -(_a * _startingPoint.x + _b * _startingPoint.y);
	}
}

const cv::Point2d& sb::Line::getStartingPoint() const { return _startingPoint; }

void sb::Line::setStartingPoint( const cv::Point2d& startingPoint )
{
	if ( _startingPoint == startingPoint ) return;

	_startingPoint = startingPoint;
	regenerateCoefs();
}

const cv::Point2d& sb::Line::getEndingPoint() const { return _endingPoint; }

void sb::Line::setEndingPoint( const cv::Point2d& endingPoint )
{
	if ( _endingPoint == endingPoint ) return;

	_endingPoint = endingPoint;
	regenerateCoefs();
}

bool sb::Line::isValid() const { return _a != 0 || _b != 0; }

cv::Point2d sb::Line::getMiddlePoint() const { return (_startingPoint + _endingPoint) * 0.5f; }

double sb::Line::getAngleWithOx( bool degree ) const
{
	double res;

	if ( _b == 0 ) res = CV_2PI / 4;
	else res = -atan( -_a / _b );

	if ( res < 0 ) res += CV_2PI / 2;

	return degree ? res * 360 / CV_2PI : res;
}

double sb::Line::getAngleWith( const sb::Line& other, bool degree ) const { return sb::Line::calculateAngle( *this, other, degree ); }

double sb::Line::getLength() const { return sb::Line::calculateLength( *this ); }

double sb::Line::getDistance( const cv::Point2d& point ) const { return sb::Line::calculateDistance( *this, point ); }

sb::Line sb::Line::rotate( double alpha, bool degree ) const { return sb::Line::rotate( *this, alpha, degree ); }

sb::Line sb::Line::rotate( const cv::Point2d& anchor, double alpha, bool degree ) const { return sb::Line::rotate( *this, anchor, alpha, degree ); }

double sb::Line::f( const cv::Point2d& point ) const { return sb::Line::f( *this, point ); }

bool sb::Line::operator==( const sb::Line& other ) const
{
	if ( _a != 0 && other._a != 0 ) {
		return _b / _a == other._b / other._a && _c / _a == other._c / other._a;
	}
	if ( _b != 0 && other._b != 0 ) {
		return _a / _b == other._a / other._b && _c / _b == other._c / other._b;
	}
	return false;
}

double sb::Line::getA() const { return _a; }

void sb::Line::setA( double a ) { _a = a; }

double sb::Line::getB() const { return _b; }

void sb::Line::setB( double b ) { _b = b; }

double sb::Line::getC() const { return _c; }

void sb::Line::setC( double c ) { _c = c; }

bool sb::Line::findIntersection( const sb::Line& l1,
                                 const sb::Line& l2,
                                 cv::Point2d& output )
{
	double d = l1._b * l2._a - l2._b * l1._a;
	double dx = l1._c * l2._b - l2._c * l1._b;
	double dy = l2._c * l1._a - l1._c * l2._a;

	if ( d == 0 ) {
		return false;
	}

	output.x = (dx / d);
	output.y = (dy / d);

	return true;
}

double sb::Line::calculateAngle( const sb::Line& l1,
                                 const sb::Line& l2,
                                 bool degree )
{
	double res = abs( l1.getAngleWithOx( degree ) - l2.getAngleWithOx( degree ) );
	return res > 90 ? 180 - res : res;
}

sb::Line sb::Line::rotate( const sb::Line& sourceLine,
                           double alpha,
                           bool degree )
{
	return sb::Line::rotate( sourceLine, sourceLine.getMiddlePoint(), alpha, degree );
}

sb::Line sb::Line::rotate( const sb::Line& sourceLine,
                           const cv::Point2d& anchor,
                           double alpha,
                           bool degree )
{
	if ( !degree ) alpha = alpha * 360 / CV_2PI;

	double a0 = sourceLine.getAngleWithOx() + alpha;

	return a0 == 90 ? sb::Line( 1, 0, anchor ) :
		       sb::Line( tan( -a0 * CV_2PI / 360 ), -1, anchor );
}

double sb::Line::f( const sb::Line& sourceLine,
                    const cv::Point2d& point ) { return sourceLine._a * point.x + sourceLine._b * point.y + sourceLine._c; }

void sb::Line::regenerateCoefs()
{
	double a = _startingPoint.x - _endingPoint.x;
	double b = _startingPoint.y - _endingPoint.y;
	_a = b;
	_b = -a;

	_c = -(_a * _startingPoint.x + _b * _startingPoint.y);
}

void sb::Line::write( cv::FileStorage& fs ) const
{
	fs
			<< "{"
			<< "A" << _a
			<< "B" << _b
			<< "C" << _c
			<< "StartingPoint" << _startingPoint
			<< "EndingPoint" << _endingPoint
			<< "}";
}

void sb::Line::read( const cv::FileNode& node )
{
	node["A"] >> _a;
	node["B"] >> _b;
	node["C"] >> _c;
	node["StartingPoint"] >> _startingPoint;
	node["EndingPoint"] >> _endingPoint;
}

void sb::write( cv::FileStorage& fs, const std::string&, const sb::Line& data )
{
	data.write( fs );
}

void sb::read( const cv::FileNode& node, sb::Line& data, const sb::Line& defaultData )
{
	if ( node.empty() ) data = defaultData;
	else data.read( node );
}

double sb::Line::calculateLength( const sb::Line& sourceLine )
{
	return sqrt( (sourceLine._startingPoint.x - sourceLine._endingPoint.x) * (sourceLine._startingPoint.x - sourceLine._endingPoint.x)
	            + (sourceLine._startingPoint.y - sourceLine._endingPoint.y) * (sourceLine._startingPoint.y - sourceLine._endingPoint.y) );
}

double sb::Line::calculateDistance( const sb::Line& sourceLine,
                                    const cv::Point2d point )
{
	return abs( sourceLine.f( point ) ) / sqrt( sourceLine._a * sourceLine._a + sourceLine._b * sourceLine._b );
}
