#ifndef __SB_ANALYZER_H__
#define __SB_ANALYZER_H__

#include "../Params.h"
#include "../Timer.h"
#include "../calculator/FrameInfo.h"
#include "LaneComponent.h"
#include "RoadInfo.h"

#define ANALYZER_FIRST_ANALYZE_TIMEOUT 5
#define ANALYZER_TRACK_ANALYZE_TIMEOUT 3

namespace sb
{
class Analyzer
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
	int _firstAnalyzeTimes;

	int _trackAnalyzeTimes;

	int firstAnalyze( const sb::FrameInfo& frameInfo,
	                  sb::RoadInfo& roadInfo );


	int trackAnalyze( const sb::FrameInfo& frameInfo,
										sb::RoadInfo& roadInfo );
};
}

#endif //!__SB_ANALYZER_H__
