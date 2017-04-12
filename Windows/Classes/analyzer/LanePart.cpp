#include "LanePart.h"

void sb::release( sb::LanePartInfo* lanePartInfo )
{
	delete lanePartInfo->part;
	lanePartInfo->part = nullptr;
}
