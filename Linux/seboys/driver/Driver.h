#ifndef __SB_DRIVER_H__
#define __SB_DRIVER_H__

#include "Params.h"
#include "analyzer/RoadInfo.h"
#include "driver/Car.h"

namespace sb {
    class Driver {
    private:
        sb::Car *car;

        float current_thera_road; // xu huong duong hien tai

    public:
        Driver() : direction(DIR_FORWARD),
                   velocity(30),
                   theta(0),
                   current_direction(DIR_BRAKE),
                   current_thera_road(0) {}

        int init(const sb::Params &params);

        int drive(const sb::RoadInfo &roadInfo);

        void release();
    };
}

#endif //!__SB_DRIVER_H__
