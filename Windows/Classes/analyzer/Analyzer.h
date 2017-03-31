#ifndef __SB_ANALYZER_H__
#define __SB_ANALYZER_H__

#include "../Params.h"
#include "../calculator/FrameInfo.h"
#include "../calculator/Formatter.h"
#include "RoadInfo.h"

namespace sb
{
class Analyzer
{
private:
	double _laneWidth = 6;
	double _roadWidth = 80;

	sb::Formatter _debugFormatter;

public:
	Analyzer() {}

	int init( const sb::Params& params );

	int analyze( const sb::FrameInfo& frameInfo,
	             sb::RoadInfo& roadInfo ) const;

	void release();
};
}

#endif //!__SB_ANALYZER_H__
