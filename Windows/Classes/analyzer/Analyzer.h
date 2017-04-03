#ifndef __SB_ANALYZER_H__
#define __SB_ANALYZER_H__

#include "../Params.h"
#include "../calculator/FrameInfo.h"
#include "../calculator/Formatter.h"
#include "RoadInfo.h"

#include <stack>
#include <vector>

#define MAX_ACCEPTABLE_ANGLE_DIFF 5

namespace sb
{
struct LanePart
{
	cv::Point2d vertices[4];

	cv::Point2d position;

	double width;

	double angle;

	LanePart()
		: width( 0 ), angle( 0 ), position( 0, 0 ) {}

	LanePart( const cv::Point2d& _position, double _angle, double _width, double _length = -1 );

	bool operator==( const sb::LanePart& other ) const;

	void calculateVertices( double _length );
};

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

	sb::Formatter _debugFormatter;

public:
	int init( const sb::Params& params );

	int analyze( const sb::FrameInfo& frameInfo,
	             sb::RoadInfo& roadInfo ) const;

	void release();

private:
	int move_window( cv::Rect2d& window ) const;

	bool segment_intersect_rectangle( const cv::Point2d& p1,
	                                  const cv::Point2d& p2,
	                                  const cv::Rect2d& rect ) const;

	int find_lines_inside_window( const std::vector<sb::LineInfo>& inputLines,
	                              const cv::Rect2d& window,
	                              std::vector<sb::LineInfo>& outputLines ) const;

	int find_first_lane_parts( const std::vector<sb::LineInfo>& lines,
	                           std::vector<sb::LanePart>& lane_parts ) const;

	int find_next_lane_parts( const std::vector<sb::LineInfo>& lines,
	                          const sb::LanePart& lastest_lane_part,
	                          std::vector<sb::LanePart>& lane_parts,
	                          std::vector<double>& lane_part_ratings ) const;

	int draw_lane_part( const sb::LanePart& lane_part, cv::Mat& image,
	                    const cv::Size& expand_size,
	                    const cv::Scalar& color,
	                    int line_width ) const;

	int calculate_full_lane_parts( const sb::LanePart& first_lane_part,
	                               const std::vector<sb::LineInfo>& full_lines_list,
																 std::vector<sb::LanePart>& output_full_lane_parts,
	                               const cv::Mat& image,
	                               const cv::Size& expand_size ) const;

	int analyze3( const sb::FrameInfo& frameInfo,
	              sb::RoadInfo& roadInfo ) const;
};
}

#endif //!__SB_ANALYZER_H__
