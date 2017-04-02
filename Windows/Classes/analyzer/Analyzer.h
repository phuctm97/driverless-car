#ifndef __SB_ANALYZER_H__
#define __SB_ANALYZER_H__

#include "../Params.h"
#include "../calculator/FrameInfo.h"
#include "../calculator/Formatter.h"
#include "RoadInfo.h"

#define MAX_ACCEPTABLE_ANGLE_DIFF 5

namespace sb
{
class Analyzer
{
private:
	double _minLandWidth;
	double _maxLandWidth;
	double _minRoadWidth;
	double _maxRoadWidth;

	sb::Formatter _debugFormatter;

public:
	Analyzer() {}

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

	int analyze4( const sb::FrameInfo& frameInfo,
	              sb::RoadInfo& roadInfo ) const;

	bool segmentIntersectRectangle( const cv::Point2d& p1,
	                                const cv::Point2d& p2,
	                                const cv::Rect2d& rect ) const;

	void drawCandidate( const cv::Vec6d& candidate ) const;
};
}

#endif //!__SB_ANALYZER_H__
