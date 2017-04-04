#ifndef __SB_ANALYZER_H__
#define __SB_ANALYZER_H__

#include "../Params.h"
#include "../Timer.h"
#include "../calculator/FrameInfo.h"
#include "../calculator/Formatter.h"
#include "Lane.h"
#include "RoadInfo.h"

#include <stack>
#include <vector>

#define MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_LINES 5
#define MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_LANE_PARTS 2
#define MAX_ACCEPTABLE_DISTANCE_BETWEEN_LANE_PARTS 2
#define MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_LANE_PARTS 20

namespace sb
{
class Analyzer
{
private:
	double _minLandWidth;
	double _maxLandWidth;
	double _minRoadWidth;
	double _maxRoadWidth;

	cv::Size _windowSize;
	cv::Point2d _windowMove;
	cv::Point2d _topRightCorner;
	double _lanePartLength;

	sb::Formatter _debugFormatter;

public:
	int init( const sb::Params& params );

	int analyze( const sb::FrameInfo& frameInfo,
	             sb::RoadInfo& roadInfo ) const;

	void release();

private:
	int analyze3( const sb::FrameInfo& frameInfo,
	              sb::RoadInfo& roadInfo ) const;

	int move_window( cv::Rect2d& window ) const;

	int find_lines_intersect_rectangle( const std::vector<sb::LineInfo>& inputLines,
	                                    const cv::Rect2d& window,
	                                    std::vector<sb::LineInfo>& outputLines ) const;

	int find_first_lane_parts( const std::vector<sb::LineInfo>& lines,
	                           std::vector<sb::LanePart>& lane_parts ) const;

	int find_next_lane_parts( const std::vector<sb::LineInfo>& lines,
	                          const sb::LanePart& lastest_lane_part,
	                          std::vector<sb::LanePart>& lane_parts,
	                          std::vector<double>& lane_part_ratings ) const;

	int calculate_full_lane( const sb::LanePart& first_lane_part,
	                         const std::vector<sb::LineInfo>& full_lines_list,
	                         sb::Lane& output_lane,
	                         const cv::Mat& image,
	                         const cv::Size& expand_size ) const;

	int draw_lane_part( const sb::LanePart& lane_part, cv::Mat& image,
	                    const cv::Size& expand_size,
	                    const cv::Scalar& color,
	                    int line_width ) const;

	bool segment_intersect_rectangle( const cv::Point2d& p1,
	                                  const cv::Point2d& p2,
	                                  const cv::Rect2d& rect ) const;
};
}

#endif //!__SB_ANALYZER_H__
