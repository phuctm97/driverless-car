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

<<<<<<< HEAD
	void write( cv::FileStorage& fs ) const;
};
=======
	void create( const Params& params );

	void setObstacleBoxes( const std::vector<cv::Rect2d>& obstacleBoxes );

	const std::vector<cv::Rect2d>& getObstacleBoxes() const;

	void setLeftKnots( const std::vector<cv::Point2d>& leftKnots );

	const std::vector<cv::Point2d>& getLeftKnots() const;

	void setRightKnots( const std::vector<cv::Point2d>& rightKnots );
>>>>>>> master

void create( sb::RoadInfo* roadInfo, Params* params );

void release( sb::RoadInfo* roadInfo );

void write( cv::FileStorage& fs, const std::string&, const sb::RoadInfo& data );

<<<<<<< HEAD
=======
	void read( const cv::FileNode& node );

	void write( cv::FileStorage& fs ) const;
};

void write( cv::FileStorage& fs, const std::string&, const sb::RoadInfo& data );

>>>>>>> master
void read( const cv::FileNode& node, sb::RoadInfo& data, const sb::RoadInfo& defaultData = sb::RoadInfo() );
}

#endif //!__SB_ROAD_INFO_H__
