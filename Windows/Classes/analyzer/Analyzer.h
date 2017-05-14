#ifndef __SB_ANALYZER_H__
#define __SB_ANALYZER_H__

#include "../Params.h"
#include "../Timer.h"
#include "../calculator/FrameInfo.h"
#include "RoadInfo.h"
#include "Repository.h"

//#define SB_DEBUG_ANALYZER

#define ANALYZER_FIRST_ANALYZE_TIMEOUT 5
#define ANALYZER_TRACK_ANALYZE_TIMEOUT 3
#define MIN_ACCEPTABLE_FULL_LANE_BLOB_OBJECTS_COUNT 1500
#define MIN_ACCEPTABLE_CHILD_BLOB_OBJECTS_COUNT 10
#define MAX_ACCEPTABLE_LANE_POSITION_DIFF 100

namespace sb
{
enum RoadState
{
	BOTH_LANE_DETECTED,
	IGNORE_LEFT_LANE,
	IGNORE_RIGHT_LANE,
	UNKNOWN
};

struct LaneKnot
{
	cv::Point position = cv::Point();
	int type = 0;
};

struct Analyzer
{
	sb::Repository* repo;

	std::vector<std::pair<sb::LaneKnot, sb::LaneKnot>> knots;

	int roadState;

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

void findLanes( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo );

void findBothLanes( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo );

void trackLanes( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo );

void trackBothLanes( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo );

void trackLeftLane( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo );

void trackRightLane( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo );

void analyzeWithBothLane( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo, sb::RoadInfo* roadInfo );

void analyzeWithoutLeftLane( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo, sb::RoadInfo* roadInfo );

void analyzeWithoutRightLane( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo, sb::RoadInfo* roadInfo );

void analyzeResult( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo, sb::RoadInfo* roadInfo );

void release( sb::Analyzer* analyzer );
}

#endif //!__SB_ANALYZER_H__

