#ifndef __SB_DRIVER_H__
#define __SB_DRIVER_H__

#include <opencv2/opencv.hpp>
#include "driver/Car.h"

class Driver {
private:
    sb::Car *car;
    float cur_road_theta; // xu huong duong hien tai

public:
    Driver() {}

    int init(const sb::Params &params);

    int drive(const sb::RoadInfo &roadInfo);

    void release();
};


#endif //!__SB_DRIVER_H__
