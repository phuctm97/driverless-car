#include "RoadInfo.h"

const cv::Point2d& sb::RoadInfo::getPositionOfLeftLane() const { return _positionOfLeftLane; }

const cv::Point2d& sb::RoadInfo::getPositionOfRightLane() const { return _positionOfRightLane; }

const std::vector<cv::Rect2d>& sb::RoadInfo::getObstacleBoxes() const { return _obstacleBoxes; }

double sb::RoadInfo::getRotationOfLane() const { return _rotationOfLane; }

float sb::RoadInfo::getReliability() const { return _reliability; }
