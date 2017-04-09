#ifndef __SB_LANE_COMPONENT_H__
#define __SB_LANE_COMPONENT_H__

#include "../Include.h"
#include "../calculator/FrameInfo.h"
#include "LanePart.h"
#include "Lane.h"

#include <stack>

#define MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_LINES 5
#define MAX_ACCEPTABLE_VERTICAL_DIFF_BETWEEN_LINES 5
#define MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_LANE_PARTS 20
#define MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_LANE_PARTS 2
#define MAX_ACCEPTABLE_DISTANCE_BETWEEN_LANE_PARTS 2

#define NUM_LANE_PARTS 7
#define GOOD_LANE_RATING 7.5
#define GOOD_ROAD_RATING 7
#define BAD_LINE_RATING 4.0

#ifdef SB_DEBUG
#include "../calculator/Formatter.h"
#endif

namespace sb
{
class LaneComponent
{
private:
	int _side;

	cv::Point2d _position;

	double _width;

	double _angle;

	std::vector<sb::LanePart> _parts;

	double _minLandWidth;

	double _maxLandWidth;

	cv::Size _windowSize;

	cv::Point2d _windowMove;

	cv::Point2d _windowMaxXY;

	double _lanePartLength;

	bool _initSucceeded;

public:

	void init( int side, double minLaneWidth, double maxLaneWidth );

	int findItself( const sb::FrameInfo& frameInfo );

	int track( const sb::FrameInfo& frameInfo );

private:

#ifdef SB_DEBUG
	void findFirstLaneParts( const std::vector<sb::LineInfo>& line_lists,
	                         const cv::Rect2d& window,
	                         std::vector<sb::LanePart>& first_lane_parts,
	                         const cv::Mat& image,
	                         const cv::Size& expand_size ) const;
#else
	void findFirstLaneParts( const std::vector<sb::LineInfo>& line_lists,
													 const cv::Rect2d& window,
													 std::vector<sb::LanePart>& first_lane_parts ) const;
#endif // findFirstLaneParts()

#ifdef SB_DEBUG
	void findNextLaneParts( const std::vector<sb::LineInfo>& lines_list,
	                        const sb::LanePart& lastest_lane_part,
	                        std::vector<sb::LanePart>& next_lane_parts,
	                        std::vector<double>& next_lane_part_ratings,
	                        const cv::Mat& image,
	                        const cv::Size& expand_size ) const;
#else
	void findNextLaneParts( const std::vector<sb::LineInfo>& lines_list,
													const sb::LanePart& lastest_lane_part,
													std::vector<sb::LanePart>& next_lane_parts,
													std::vector<double>& next_lane_part_ratings ) const;
#endif // findNextLaneParts()

#ifdef SB_DEBUG
	void findLanes( const std::vector<sb::LineInfo>& lines_list,
	                const sb::LanePart& first_lane_part,
	                std::vector<sb::Lane>& lanes,
	                const cv::Mat& image,
	                const cv::Size& expand_size ) const;
#else
	void findLanes( const std::vector<sb::LineInfo>& lines_list,
									const sb::LanePart& first_lane_part,
									std::vector<sb::Lane>& lanes ) const;
#endif // findLanes()

	int findLinesIntersectRectangle( const std::vector<sb::LineInfo>& inputLines,
	                                 const cv::Rect2d& window,
	                                 std::vector<sb::LineInfo>& outputLines ) const;

	bool segmentIntersectRectangle( const cv::Point2d& p1,
	                                const cv::Point2d& p2,
	                                const cv::Rect2d& rect ) const;

	int moveFirstWindow( cv::Rect2d& window ) const;
};
}

#endif //!__SB_LANE_COMPONENT_H__
