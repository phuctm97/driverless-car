#ifndef __SB_LINE_INFO_H__
#define __SB_LINE_INFO_H__

#include "Line.h"

namespace sb
{
class LineInfo
{
private:
	sb::Line _line;

	double _length;

	double _angle;

	cv::Point2d _middlePoint;

	cv::Vec3b _averageColor;

public:
	LineInfo( const sb::Line& line,
	          const cv::Vec3b& averageColor = cv::Vec3b( 0, 0, 0 ) );

	const sb::Line& getLine() const;

	void setLine( const sb::Line& line );

	const cv::Point2d& getStartingPoint() const;

	const cv::Point2d& getEndingPoint() const;

	double getLength() const;

	double getAngle() const;

	const cv::Vec3b& getAverageColor() const;

	const cv::Point2d& getMiddlePoint() const;

	void setAverageColor( const cv::Vec3b& averageColor );
};
}

#endif //!__SB_LINE_INFO_H__

