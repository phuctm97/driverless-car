#include "MindedDriver.h"

int sb::MindedDriver::firstSee( const sb::FrameInfo& frame )
{
	_leftLane.findItself( frame );
	_rightLane.findItself( frame );

	return 0;
}

int sb::MindedDriver::observe( const sb::FrameInfo& frame )
{

}
