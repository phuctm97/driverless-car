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

	cv::Vec3b _averageColor;

public:
	LineInfo( const sb::Line& line );

	LineInfo( const cv::Mat& colorImage, const sb::Line& line );

	const sb::Line& getLine() const;

	const cv::Point2d& getStartingPoint() const;

	const cv::Point2d& getEndingPoint() const;

	double getLength() const;

	double getAngle() const;

	const cv::Vec3b& getAverageColor() const;

private:
	void calculateAverageColor( const cv::Mat& colorImage );
};
}

#endif //!__SB_LINE_INFO_H__
