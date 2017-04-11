#ifndef __SB_ROAD_INFO_H__
#define __SB_ROAD_INFO_H__

#include "../Include.h"
#include "../Params.h"
#include "../calculator/LineInfo.h"

namespace sb
{
struct RoadInfo
{
	cv::Point target;

	std::vector<cv::Point> leftKnots;

	std::vector<cv::Point> rightKnots;

	std::vector<cv::Rect> obstacleBoxes;

	void read( const cv::FileNode& node );

	void write( cv::FileStorage& fs ) const;
};

void create( sb::RoadInfo* roadInfo, Params* params );

void write( cv::FileStorage& fs, const std::string&, const sb::RoadInfo& data );

void read( const cv::FileNode& node, sb::RoadInfo& data, const sb::RoadInfo& defaultData = sb::RoadInfo() );
}

#endif //!__SB_ROAD_INFO_H__
