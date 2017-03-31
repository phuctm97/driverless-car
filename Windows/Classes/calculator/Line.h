#ifndef __SB_LINE_H__
#define __SB_LINE_H__

#include "../Include.h"

namespace sb
{
// Line Mapping: ax + by + c = 0
class Line
{
private:
	// coefficient a
	double _a;
	// coefficient b
	double _b;
	// coefficient c
	double _c;

	// segment starting point
	cv::Point2d _startingPoint;

	// segment ending point
	cv::Point2d _endingPoint;

public:
	explicit Line( const cv::Point2d& point1,
	               const cv::Point2d& point2 );

	explicit Line( double a = 0, double b = 0, double c = 0 )
		: _a( a ),
		  _b( b ),
		  _c( c ),
		  _startingPoint( 0, 0 ),
		  _endingPoint( 0, 0 ) {}

	explicit Line( double a, double b, const cv::Point2d& startingPoint )
		: _a( a ),
		  _b( b ),
		  _startingPoint( startingPoint ),
		  _endingPoint( 0, 0 ),
		  _c( -(_a * startingPoint.x + _b * startingPoint.y) ) {}

	explicit Line( double alpha, const cv::Point2d& p, bool degree = true );

	double getA() const;

	void setA( double a );

	double getB() const;

	void setB( double b );

	double getC() const;

	void setC( double c );

	const cv::Point2d& getStartingPoint() const;

	void setStartingPoint( const cv::Point2d& startingPoint );

	const cv::Point2d& getEndingPoint() const;

	void setEndingPoint( const cv::Point2d& endingPoint );

	bool isValid() const;

	cv::Point2d getMiddlePoint() const;

	double getAngleWithOx( bool degree = true ) const;

	double getAngleWith( const sb::Line& other, bool degree = true ) const;

	double getLength() const;

	double getDistance( const cv::Point2d& point ) const;

	sb::Line rotate( double alpha,
	                 bool degree = true ) const;

	sb::Line rotate( const cv::Point2d& anchor,
	                 double alpha,
	                 bool degree = true ) const;

	double f( const cv::Point2d& point ) const;

	bool operator==( const sb::Line& other ) const;

public:
	static bool findIntersection( const sb::Line& l1,
	                              const sb::Line& l2,
	                              cv::Point2d& output );

	static double calculateAngle( const sb::Line& l1,
	                              const sb::Line& l2,
	                              bool degree = true );

	static double calculateLength( const sb::Line& sourceLine );

	static double calculateDistance( const sb::Line& sourceLine,
	                                 const cv::Point2d point );

	static sb::Line rotate( const sb::Line& sourceLine,
	                        double alpha,
	                        bool degree = true );

	static sb::Line rotate( const sb::Line& sourceLine,
	                        const cv::Point2d& anchor,
	                        double alpha,
	                        bool degree = true );

	static double f( const sb::Line& sourceLine,
	                 const cv::Point2d& point );

private:
	void regenerateCoefs();

public:
	// yaml reader/writer functions
	void write( cv::FileStorage& fs ) const;

	void read( const cv::FileNode& node );
};

void write( cv::FileStorage& fs, const std::string&, const sb::Line& data );

void read( const cv::FileNode& node, sb::Line& data, const sb::Line& defaultData = sb::Line() );
}
#endif //!__SB_LINE_H__

