#ifndef __SB_LANE_PART_INFO_H__
#define __SB_LANE_PART_INFO_H__

#include "../calculator/LineInfo.h"

namespace sb
{
struct LanePart
{
	sb::LineInfo innerLine;
	sb::LineInfo outerLine;
	cv::Vec3b innerColor;
	cv::Vec3b outerColor;
	cv::Vec3b laneColor;
};

enum LanePartErrorCode
{
	BOTH_LINE_ATTACHED,
	OUTER_LINE_LOST,
	INNER_LINE_LOST,
	BOTH_LINE_LOST,
	OUTSIGHT,
	OVERLAYED,
	UNKNOWN
};

struct LanePartInfo
{
	LanePart part;

	double errorWidth;

	double errorPosition;

	double errorAngle;

	double errorColor;

	int errorCode;

	// TODO : errorColor
};
}

#endif //!__SB_LANE_PART_INFO_H__
