#ifndef __SB_ANALYZER_H__
#define __SB_ANALYZER_H__

#include "../Params.h"
#include "../calculator/FrameInfo.h"
#include "../calculator/Formatter.h"
#include "RoadInfo.h"

#define MAX_ACCEPTABLE_ANGLE_DIFF 5

namespace sb
{
struct LanePart
{
	cv::Point2d position;

	double width;

	double angle;

	double length;

	LanePart( const cv::Point2d& _position, double _angle, double _width, double _length );

	bool operator==( const sb::LanePart& other ) const;
};

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

	int move_window( const cv::Point2d& window_move,
	                 const cv::Point2d& top_right_corner,
	                 cv::Rect2d& window ) const;

	int find_lines_inside_window( const std::vector<sb::LineInfo>& inputLines,
	                              const cv::Rect2d& window,
	                              std::vector<sb::LineInfo>& outputLines ) const;

	int find_first_lane_parts( const std::vector<sb::LineInfo>& lines,
	                           std::vector<sb::LanePart>& first_lane_parts ) const;

	int calculate_lane_part_vertices( const sb::LanePart& lane_part,
	                                  cv::Point2d* vertices ) const;

	int analyze3( const sb::FrameInfo& frameInfo,
	              sb::RoadInfo& roadInfo ) const;

	int analyze4( const sb::FrameInfo& frameInfo,
	              sb::RoadInfo& roadInfo ) const;

	bool segmentIntersectRectangle( const cv::Point2d& p1,
	                                const cv::Point2d& p2,
	                                const cv::Rect2d& rect ) const;

	bool segmentIntersectCircle( const cv::Point2d& p1,
	                             const cv::Point2d& p2,
	                             const cv::Point2d& circle_origin,
	                             double circle_radius ) const;

	void drawCandidate( const cv::Vec6d& candidate ) const;
};
}

#endif //!__SB_ANALYZER_H__
