#ifndef __SB_ROAD_INFO_H__
#define __SB_ROAD_INFO_H__

#include "../Include.h"
#include "../Params.h"

namespace sb
{
class RoadInfo
{
private:
	double _positionOfLeftLane;

	double _positionOfRightLane;

	std::vector<double> _rotationOfLanes;

	std::vector<cv::Rect2d> _obstacleBoxes;

	std::vector<cv::Point2d> _leftLaneKnots;

	float _reliability;

public:
	RoadInfo()
		: _reliability( 0 ) {}

	void create( const Params& params );

	void setPositionOfLeftLane( double positionOfLeftLane );
	
	double getPositionOfLeftLane() const;

	void setPositionOfRightLane( double positionOfRightLane );
	
	double getPositionOfRightLane() const;

	void setObstacleBoxes( const std::vector<cv::Rect2d>& obstacleBoxes );
	
	const std::vector<cv::Rect2d>& getObstacleBoxes() const;
	
	std::vector<cv::Rect2d>& getObstacleBoxes();

	void setRotationOfLanes( const std::vector<double>& rotationOfLanes );

	const std::vector<double>& getRotationOfLanes() const;

	void setReliability( float reliability );

	float getReliability() const;

};
}

#endif //!__SB_ROAD_INFO_H__
