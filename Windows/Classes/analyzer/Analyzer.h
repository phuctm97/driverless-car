#ifndef __SB_ANALYZER_H__
#define __SB_ANALYZER_H__

#include "../Params.h"
#include "../Timer.h"
#include "../calculator/FrameInfo.h"
#include "LaneComponent.h"
#include "RoadInfo.h"

#define SB_DEBUG_ANALYZER

#define ANALYZER_FIRST_ANALYZE_TIMEOUT 5
#define ANALYZER_TRACK_ANALYZE_TIMEOUT 3

namespace sb
{
struct Analyzer
{
	sb::LaneComponent* leftLane;
	sb::LaneComponent* rightLane;

	std::vector<int> roadWidths;

	int firstAnalyzeTimes;
	int trackAnalyzeTimes;
};

int init( sb::Analyzer* analyzer, sb::Params* params );

int analyze( sb::Analyzer* analyzer,
             sb::FrameInfo* frameInfo,
             sb::RoadInfo* roadInfo );

int firstAnalyze( sb::Analyzer* analyzer,
                  sb::FrameInfo* frameInfo,
                  sb::RoadInfo* roadInfo );

int trackAnalyze( sb::Analyzer* analyzer,
                  sb::FrameInfo* frameInfo,
                  sb::RoadInfo* roadInfo );

void release( sb::Analyzer* analyzer );
}

#endif //!__SB_ANALYZER_H__
