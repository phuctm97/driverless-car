#include "RoadInfo.h"

const cv::Point2d &RoadInfo::getPositionOfLeftLane() const { return _positionOfLeftLane; }

const cv::Point2d &RoadInfo::getPositionOfRightLane() const { return _positionOfRightLane; }

const std::vector<cv::Rect2d> &RoadInfo::getObstacleBoxes() const { return _obstacleBoxes; }

double RoadInfo::getRotationOfLane() const { return _rotationOfLane; }

float RoadInfo::getReliability() const { return _reliability; }
