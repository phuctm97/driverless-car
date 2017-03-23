#ifndef __SB_ROAD_INFO_H__
#define __SB_ROAD_INFO_H__

#include "../Include.h"
#include "../Params.h"

namespace sb
{
class RoadInfo
{
private:
	cv::Point2d _positionOfLeftLane;

	cv::Point2d _positionOfRightLane;

	double _rotationOfLane;

	std::vector<cv::Rect2d> _obstacleBoxes;

	float _reliability;

public:
	RoadInfo()
		: _rotationOfLane( 0 ),
		  _reliability( 0 ) {}

	void create( const Params& params );

	void setPositionOfLeftLane( const cv::Point2d& positionOfLeftLane );
	
	const cv::Point2d& getPositionOfLeftLane() const;

	void setPositionOfRightLane( const cv::Point2d& positionOfRightLane );
	
	const cv::Point2d& getPositionOfRightLane() const;

	void setObstacleBoxes( const std::vector<cv::Rect2d>& obstacleBoxes );
	
	const std::vector<cv::Rect2d>& getObstacleBoxes() const;
	
	std::vector<cv::Rect2d>& getObstacleBoxes();

	void setRotationOfLane( double rotationOfLane );

	double getRotationOfLane() const;
	
	void setReliability( float reliability );

	float getReliability() const;

};
}

#endif //!__SB_ROAD_INFO_H__
