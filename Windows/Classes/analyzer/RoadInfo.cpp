#include "RoadInfo.h"

void sb::RoadInfo::create( const Params& params )
{
}

void sb::RoadInfo::setPositionOfLeftLane( double positionOfLeftLane ) { _positionOfLeftLane = positionOfLeftLane; }

double sb::RoadInfo::getPositionOfLeftLane() const { return _positionOfLeftLane; }

void sb::RoadInfo::setPositionOfRightLane( double positionOfRightLane ) { _positionOfRightLane = positionOfRightLane; }

double sb::RoadInfo::getPositionOfRightLane() const { return _positionOfRightLane; }

void sb::RoadInfo::setObstacleBoxes( const std::vector<cv::Rect2d>& obstacleBoxes ) { _obstacleBoxes = obstacleBoxes; }

const std::vector<cv::Rect2d>& sb::RoadInfo::getObstacleBoxes() const { return _obstacleBoxes; }

std::vector<cv::Rect2d>& sb::RoadInfo::getObstacleBoxes() { return _obstacleBoxes; }

void sb::RoadInfo::setRotationOfLanes( const std::vector<double>& rotationOfLanes ) { _rotationOfLanes = rotationOfLanes; }

const std::vector<double>& sb::RoadInfo::getRotationOfLanes() const { return _rotationOfLanes; }

void sb::RoadInfo::setReliability( float reliability ) { _reliability = reliability; }

float sb::RoadInfo::getReliability() const { return _reliability; }
