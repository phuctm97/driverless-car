#ifndef __SB_LANE_PART_INFO_H__
#define __SB_LANE_PART_INFO_H__

#include "../calculator/LineInfo.h"

namespace sb
{
struct LanePart
{
	cv::Point origin;

	cv::Rect box;

	cv::Vec3b bgr;
};

enum LanePartErrorCode
{
	PART_NICE,
	PART_OUTSIGHT_LEFT,
	PART_OUTSIGHT_RIGHT,
	PART_OVERLAYED,
	PART_UNKNOWN
};

struct LanePartInfo
{
	LanePart* part;

	int errorWidth;

	int errorPosition;

	float errorAngle;

	float errorColor;

	int errorCode;
};

void release( sb::LanePartInfo* lanePartInfo );
}

#endif //!__SB_LANE_PART_INFO_H__
