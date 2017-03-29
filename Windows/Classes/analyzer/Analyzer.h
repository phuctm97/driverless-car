#ifndef __SB_ANALYZER_H__
#define __SB_ANALYZER_H__

#include "../Params.h"
#include "../calculator/FrameInfo.h"
#include "RoadInfo.h"

namespace sb
{
class Analyzer
{
private:
	int _whiteThresh = 190;

public:
	Analyzer() {}

	int init( const sb::Params& params );

	int analyze( const sb::FrameInfo& frameInfo,
	             sb::RoadInfo& roadInfo ) const;

	void release();

private:
	int analyze1( const sb::FrameInfo& frameInfo,
	              sb::RoadInfo& roadInfo ) const;

};
}

#endif //!__SB_ANALYZER_H__
