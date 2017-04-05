#ifndef __SB_ANALYZER_H__
#define __SB_ANALYZER_H__

 #define SB_DEBUG

#include "../Params.h"
#include "../Timer.h"
#include "../calculator/FrameInfo.h"
#include "../calculator/Formatter.h"
#include "Lane.h"
#include "Road.h"
#include "RoadInfo.h"

#include <stack>
#include <vector>

#define MAX_ACCEPTABLE_DISTANCE_BETWEEN_LANE_PARTS 2
#define MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_LINES 5
#define MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_LANE_PARTS 20
#define MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_LANE_PARTS 2
#define MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_TWO_LANES 10
#define MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_TWO_LANES 5
#define NUM_LANE_PARTS 7

#define GOOD_LANE_RATING 7.5
#define GOOD_ROAD_RATING 7
#define ANALYZER_TIMEOUT 500

// TODO
// - Tính toán lại rating chuẩn cho road từ đó xác định độ tin cậy cũng nên điểm nên đến
// - Xuất road info + target dựa trên độ tin cậy cho driver


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

	double _learntLaneWidth;
	double _learntRoadWidth;

	sb::Formatter _debugFormatter;

public:
	int init( const sb::Params& params );

	int analyze( const sb::FrameInfo& frameInfo,
	             sb::RoadInfo& roadInfo );

	void release();

private:
	int analyze3( const sb::FrameInfo& frameInfo,
	              sb::RoadInfo& roadInfo );

	bool segmentIntersectRectangle( const cv::Point2d& p1,
	                                const cv::Point2d& p2,
	                                const cv::Rect2d& rect ) const;

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
#endif //SB_DEBUG

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
#endif //SB_DEBUG

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
#endif //SB_DEBUG

#ifdef SB_DEBUG
	void findRoads( const std::vector<sb::LineInfo>& lines_list,
	                const sb::Lane& lane,
	                std::vector<sb::Road>& roads,
	                const cv::Mat& image,
	                const cv::Size& expand_size ) const;
#else
	void findRoads( const std::vector<sb::LineInfo>& lines_list,
									const sb::Lane& lane,
									std::vector<sb::Road>& roads ) const;
#endif //SB_DEBUG

#ifdef SB_DEBUG
	void findPossibleSideAndRoadWidths( const std::vector<sb::LineInfo>& lines_list,
	                                    const sb::Lane& lane,
	                                    std::vector<std::pair<int, double>>& scenarios,
	                                    const cv::Mat& image,
	                                    const cv::Size& expand_size ) const;
#else
	void findPossibleSideAndRoadWidths( const std::vector<sb::LineInfo>& lines_list,
																			const sb::Lane& lane,
																			std::vector<std::pair<int, double>>& scenarios) const;
#endif //SB_DEBUG

	void calculateRoadTarget( const sb::Road& final_road, sb::RoadInfo& roadInfo ) const;

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
