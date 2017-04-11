#ifndef __SB_LINE_INFO_H__
#define __SB_LINE_INFO_H__

#include "Line.h"

namespace sb
{
struct LineInfo
{
	// TODO: to float

	sb::Line line;

	double length;

	double angle;

	cv::Point2d middlePoint;

	cv::Point2d topPoint;

	cv::Point2d bottomPoint;

	cv::Point2d centerPoint;
};

void create( sb::LineInfo* lineInfo, const sb::Line& line );

}

#endif //!__SB_LINE_INFO_H__
