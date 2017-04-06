#ifndef __SB_MINDED_DRIVER_H__
#define __SB_MINDED_DRIVER_H__

#include "../calculator/FrameInfo.h"
#include "LaneComponent.h"

namespace sb
{
class MindedDriver
{
private:
	sb::LaneComponent _leftLane;
	sb::LaneComponent _rightLane;

//	sb::GuardComponent	_guard;
//	std::vector<sb::StrangeComponent> _strangers;

public:
	MindedDriver() {}

	int firstSee( const sb::FrameInfo& frame );

	int observe( const sb::FrameInfo& frame );

};

}

#endif //!__SB_MINDED_DRIVER_H__
