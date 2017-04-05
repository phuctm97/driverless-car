#ifndef __SB_ROAD_INFO_H__
#define __SB_ROAD_INFO_H__

#include "../Include.h"
#include "../Params.h"
#include "../calculator/LineInfo.h"

namespace sb
{
class RoadInfo
{
private:
	cv::Point2d _target;

	std::vector<cv::Point2d> _leftKnots;

	std::vector<cv::Point2d> _rightKnots;

	std::vector<cv::Rect2d> _obstacleBoxes;

	float _reliability;

public:
	RoadInfo()
		: _reliability( 0 ) {}

	void create( const Params& params );

	void setObstacleBoxes( const std::vector<cv::Rect2d>& obstacleBoxes );

	const std::vector<cv::Rect2d>& getObstacleBoxes() const;

	void setLeftKnots( const std::vector<cv::Point2d>& leftKnots );

	const std::vector<cv::Point2d>& getLeftKnots() const;

	void setRightKnots( const std::vector<cv::Point2d>& rightKnots );

	const std::vector<cv::Point2d>& getRightKnots() const;

	void setReliability( float reliability );

	float getReliability() const;

	const cv::Point2d& getTarget() const;

	void setTarget( const cv::Point2d& point );

	void read( const cv::FileNode& node );

	void write( cv::FileStorage& fs ) const;
};

void write( cv::FileStorage& fs, const std::string&, const sb::RoadInfo& data );

void read( const cv::FileNode& node, sb::RoadInfo& data, const sb::RoadInfo& defaultData = sb::RoadInfo() );
}

#endif //!__SB_ROAD_INFO_H__
