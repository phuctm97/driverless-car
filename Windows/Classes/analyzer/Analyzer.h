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
	double _laneWidth;
	double _roadWidth;

	sb::Formatter _debugFormatter;

public:
	Analyzer()
		: _laneWidth( 0 ), _roadWidth( 0 ) {}

	int init( const sb::Params& params );

	int analyze( const sb::FrameInfo& frameInfo,
	             sb::RoadInfo& roadInfo ) const;

	void release();

private:
	int analyze1( const sb::FrameInfo& frameInfo,
	              sb::RoadInfo& roadInfo ) const;

	int analyze2( const sb::FrameInfo& frameInfo,
	              sb::RoadInfo& roadInfo ) const;

	int analyze3( const sb::FrameInfo& frameInfo,
	              sb::RoadInfo& roadInfo ) const;
};
}

#endif //!__SB_ANALYZER_H__
