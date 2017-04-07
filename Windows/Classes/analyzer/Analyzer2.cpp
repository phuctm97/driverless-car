#include "Analyzer2.h"

int sb::Analyzer2::init( const sb::Params& params )
{
	_firstAnalyzed = false;

#ifdef SB_DEBUG
	_leftLane.init( -1, params.MIN_LANE_WIDTH, params.MAX_LANE_WIDTH, params );
	_rightLane.init( 1, params.MIN_LANE_WIDTH, params.MAX_LANE_WIDTH, params );
#else
	_leftLane.init( -1, params.MIN_LANE_WIDTH, params.MAX_LANE_WIDTH );
	_rightLane.init( 1, params.MIN_LANE_WIDTH, params.MAX_LANE_WIDTH );
#endif

	return 0;
}

int sb::Analyzer2::analyze( const sb::FrameInfo& frameInfo, sb::RoadInfo& roadInfo )
{
	// first analyze
	if ( !_firstAnalyzed ) {
		_firstAnalyzed = true;
		return firstAnalyze( frameInfo, roadInfo );
	}

	if ( _leftLane.track( frameInfo ) < 0 )return -1;
	if ( _rightLane.track( frameInfo ) < 0 )return -1;

	cv::Point2d target;
	// target = (_leftLane.getKnots()[LAST] + _rightLane.getKnots()[LAST])*0.5;

	roadInfo.setTarget( target );

	return 0;
}

void sb::Analyzer2::release() {}

int sb::Analyzer2::firstAnalyze( const sb::FrameInfo& frameInfo,
                                 sb::RoadInfo& roadInfo )
{
	if ( _leftLane.findItself( frameInfo ) < 0 ) return -1;
	if ( _rightLane.findItself( frameInfo ) < 0 )return -1;

	return 0;
}
