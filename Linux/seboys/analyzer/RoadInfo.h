#ifndef __SB_ROAD_INFO_H__
#define __SB_ROAD_INFO_H__

#include "../Include.h"

class RoadInfo {
private:
    cv::Point2d _positionOfLeftLane;

    cv::Point2d _positionOfRightLane;

    double _rotationOfLane;

    std::vector<cv::Rect2d> _obstacleBoxes;

    float _reliability;

public:
    RoadInfo()
            : _rotationOfLane(0),
              _reliability(0) {}

    const cv::Point2d &getPositionOfLeftLane() const;

    const cv::Point2d &getPositionOfRightLane() const;

    const std::vector<cv::Rect2d> &getObstacleBoxes() const;

    double getRotationOfLane() const;

    float getReliability() const;
};

#endif //!__SB_ROAD_INFO_H__
