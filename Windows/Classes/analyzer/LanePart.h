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

struct LanePartInfo
{
	LanePart part;

	double errorWidth;

	double errorPosition;

	double errorAngle;

	// TODO : errorColor
};

}

#endif //!__SB_LANE_PART_INFO_H__
