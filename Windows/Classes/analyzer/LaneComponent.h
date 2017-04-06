#ifndef __SB_LANE_COMPONENT_H__
#define __SB_LANE_COMPONENT_H__

#include "../Include.h"
#include "../calculator/FrameInfo.h"
#include "LanePart.h"

namespace sb
{

class LaneComponent
{
private:
	cv::Point2d _position;
	
	double _width;

	double _angle;

	std::vector<sb::LanePart> _parts;



	double _minLandWidth;

	double _maxLandWidth;

	cv::Size _windowSize;

	cv::Point2d _windowMove;

	cv::Point2d _topRightCorner;

	double _lanePartLength;

public:
	void findItself( const sb::FrameInfo& frameInfo );

};

}

#endif //!__SB_LANE_COMPONENT_H__