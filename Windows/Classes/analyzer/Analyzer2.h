#ifndef __SB_ANALYZER_2_H__
#define __SB_ANALYZER_2_H__

#include "../Params.h"
#include "../Timer.h"
#include "../calculator/FrameInfo.h"
#include "../calculator/Formatter.h"
#include "LaneComponent.h"
#include "RoadInfo.h"

namespace sb
{
class Analyzer2
{
private:
	sb::LaneComponent _leftLane;
	sb::LaneComponent _rightLane;

public:
	int init( const sb::Params& params );

	int analyze( const sb::FrameInfo& frameInfo,
	             sb::RoadInfo& roadInfo );

	void release();

private:
	bool _firstAnalyzed;

	int firstAnalyze( const sb::FrameInfo& frameInfo,
	                  sb::RoadInfo& roadInfo );
};
}

#endif //!__SB_ANALYZER_2_H__
