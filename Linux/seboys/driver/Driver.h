#ifndef __SB_DRIVER_H__
#define __SB_DRIVER_H__

#include "Params.h"
#include "analyzer/RoadInfo.h"
#include "driver/Car.h"

class Driver {
private:
    sb::Car *car;

    int cur_velocity;
    float cur_road_theta; // xu huong duong hien tai
    double cur_theta;

public:
    Driver() {}

    int init(const sb::Params &params);

    int drive(const sb::RoadInfo &roadInfo);

    void release();
};


#endif //!__SB_DRIVER_H__
