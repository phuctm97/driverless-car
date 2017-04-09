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

	cv::Point2d _topPoint;

	cv::Point2d _bottomPoint;

	cv::Point2d _centerPoint;

public:
	LineInfo() {}

	LineInfo( const sb::Line& line );

	const sb::Line& getLine() const;

	const cv::Point2d& getStartingPoint() const;

	const cv::Point2d& getEndingPoint() const;

	double getLength() const;

	double getAngle() const;

	const cv::Point2d& getMiddlePoint() const;

	const cv::Point2d& getTopPoint() const;

	void setTopPoint( const cv::Point2d& topPoint );

	const cv::Point2d& getBottomPoint() const;

	void setBottomPoint( const cv::Point2d& bottomPoint );

	const cv::Point2d& getCenterPoint() const;

	void setCenterPoint( const cv::Point2d& centerPoint );
};
}

#endif //!__SB_LINE_INFO_H__
