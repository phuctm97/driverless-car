#ifndef __SB_ROAD_H__
#define __SB_ROAD_H__

#include "../Include.h"

namespace sb
{
struct Road
{
	std::vector<std::pair<cv::Point2d, cv::Point2d>> knots;

	double lane_width;

	double road_width;

	double rating;

	double main_lane_rating;

	int main_side;
};

}


#endif //!__SB_ROAD_H__