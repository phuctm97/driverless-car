#ifndef __SB_ANALYZER_H__
#define __SB_ANALYZER_H__

#include "../Params.h"
#include "../Timer.h"
#include "../calculator/FrameInfo.h"
#include "../calculator/Formatter.h"
#include "Lane.h"
#include "Road.h"
#include "RoadInfo.h"

#include <stack>
#include <vector>

#define MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_LINES 5
#define MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_LANE_PARTS 2
#define MAX_ACCEPTABLE_DISTANCE_BETWEEN_LANE_PARTS 2
#define MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_LANE_PARTS 20
#define MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_TWO_LANES 10

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

	int calculate_full_road( const sb::Lane& lane,
	                         double lane_rating,
	                         const std::vector<sb::LineInfo>& full_lines_list,
	                         sb::Road& road,
	                         const cv::Mat& image,
	                         const cv::Size& expand_size ) const;

	bool segmentIntersectRectangle( const cv::Point2d& p1,
	                                  const cv::Point2d& p2,
	                                  const cv::Rect2d& rect ) const;

	void findFirstLaneParts( const std::vector<sb::LineInfo>& line_lists,
	                         const cv::Rect2d& window,
	                         std::vector<sb::LanePart>& first_lane_parts,
	                         const cv::Mat& image,
	                         const cv::Size& expand_size ) const;

	void findLanes( const std::vector<sb::LineInfo>& lines_list,
	                const sb::LanePart& first_lane_part,
	                std::vector<sb::Lane>& lanes,
	                std::vector<double>& lane_ratings,
	                const cv::Mat& image,
	                const cv::Size& expand_size ) const;

	void findNextLaneParts( const std::vector<sb::LineInfo>& lines_list,
	                        const sb::LanePart& lastest_lane_part,
	                        std::vector<sb::LanePart>& next_lane_parts,
	                        std::vector<double>& next_lane_part_ratings ) const;

	void findRoads( const std::vector<sb::LineInfo>& lines_list,
	                const sb::Lane& lane,
	                const double& lane_rating,
	                std::vector<sb::Road>& roads,
	                std::vector<double>& road_ratings,
									const cv::Mat& image,
									const cv::Size& expand_size) const;

	int moveMainWindow( cv::Rect2d& window ) const;

	int drawLanePart( const sb::LanePart& lane_part, cv::Mat& image,
										const cv::Size& expand_size,
										const cv::Scalar& color,
										int line_width ) const;

	int findLinesIntersectRectangle( const std::vector<sb::LineInfo>& inputLines,
															 const cv::Rect2d& window,
															 std::vector<sb::LineInfo>& outputLines ) const;
};
}

#endif //!__SB_ANALYZER_H__
