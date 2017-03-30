#include "driver/Driver.h"

int Driver::init(const sb::Params &params) {
    this->car = new Car();

    int status = this->car->init();

    if (status < 0) {
        std::cerr << "Car init failed." << std::endl;
        return -1;
    }

    return 0;
}

int Driver::drive(const sb::RoadInfo &roadInfo) {
    int direction;
    int vel_add;
    int velocity;
    int theta; // steering angle
    int current_direction; // CURRENT DIRECTION

    float h_div_w = 0.25f * VIDEO_FRAME_HEIGHT / VIDEO_FRAME_WIDTH; // ti le 1 phan 4 height tren width

    float mid_1 = (bottom + right) / 2.0f;
    float mid_2 = (left + top) / 2.0f;
    float road_theta = atan((mid_2 - mid_1) / h_div_w); // do lech cua duong
    float theta = atan(mid_2 / h_div_w); // do lech cua xe so voi tam duong sap den

    theta = (abs(theta) > abs(road_theta)) ? theta : road_theta;

    if (abs(road_theta- cur_road_theta) < 5) {
        velocity = cur_velocity + 5;
    }
    else {
        velocity = cur_velocity + 5;
    }

    // TODO: tang, giam theo thoi gian

    // velocity in [20, 45]
    if (velocity > 45)
        velocity = 45;
    else if (velocity < 20)
        velocity = 20;

    /////////// RUN //////////////////
    if (_car.update(this->direction, vel, this->theta, this->current_direction) < 0) {
        std::cerr << "Car hardware connection corrupted." << std::endl;
        return -1;
    }
    this->current_direction = this->direction;

    return 0;
}

void Driver::release() {
    this->car->release();
    delete this->car;
    this->car = nullptr;
}
