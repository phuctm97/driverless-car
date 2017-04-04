#ifndef __SB_LANE_H__
#define __SB_LANE_H__

#include "LanePart.h"

namespace sb
{
struct Lane
{
	std::vector<sb::LanePart> parts;

	std::vector<cv::Point2d> knots;

	double width;

	int side;

	double rating;
	
	// tập hợp các lane part 
	// hoặc
	// tập hợp các knot + lane width

	// là làn trái hay phải

};

}

#endif //!__SB_LANE_H__
