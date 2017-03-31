#include "RoadInfo.h"

void sb::RoadInfo::create( const Params& params )
{
	_rotationOfLane = params.INITIAL_ROTATION_OF_LANE;
	_positionOfLeftLane = params.INITIAL_POSITION_OF_LEFT_LANE;
	_positionOfRightLane = params.INITIAL_POSITION_OF_RIGHT_LANE;
}

void sb::RoadInfo::setPositionOfLeftLane( const cv::Point2d& positionOfLeftLane ) { _positionOfLeftLane = positionOfLeftLane; }

const cv::Point2d& sb::RoadInfo::getPositionOfLeftLane() const { return _positionOfLeftLane; }

void sb::RoadInfo::setPositionOfRightLane( const cv::Point2d& positionOfRightLane ) { _positionOfRightLane = positionOfRightLane; }

const cv::Point2d& sb::RoadInfo::getPositionOfRightLane() const { return _positionOfRightLane; }

void sb::RoadInfo::setObstacleBoxes( const std::vector<cv::Rect2d>& obstacleBoxes ) { _obstacleBoxes = obstacleBoxes; }

const std::vector<cv::Rect2d>& sb::RoadInfo::getObstacleBoxes() const { return _obstacleBoxes; }

std::vector<cv::Rect2d>& sb::RoadInfo::getObstacleBoxes() { return _obstacleBoxes; }

void sb::RoadInfo::setRotationOfLane( double rotationOfLane ) { _rotationOfLane = rotationOfLane; }

double sb::RoadInfo::getRotationOfLane() const { return _rotationOfLane; }

void sb::RoadInfo::setReliability( float reliability ) { _reliability = reliability; }

float sb::RoadInfo::getReliability() const { return _reliability; }
