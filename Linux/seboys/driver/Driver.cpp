#include "driver/Driver.h"

int Driver::init(const sb::Params &params) {
    this->car = new Car();

    int status = this->car->init();
    if (status < 0) {
        std::cerr << "Car init failed." << std::endl;
        return -1;
    }

    this->cur_road_theta = 0.0;

    return 0;
}

int Driver::drive(const sb::RoadInfo &roadInfo) {
    int direction = DIR_FORWARD;

    float mid_0 = (roadInfo.getLeft(0)  + roadInfo.getRight(0)) / 2.0f;
    float mid_1 = (roadInfo.getLeft(1) + roadInfo.getRight(1)) / 2.0f;

    float h_1 = 0.25f * VIDEO_FRAME_HEIGHT / VIDEO_FRAME_WIDTH; // ti le 1 phan 4 height tren width

    float road_theta = atan((mid_1 - mid_0) / h_1); // do lech cua duong
    float theta = atan(mid_2 / h_1); // do lech cua xe so voi tam duong sap den

    theta = (abs(theta) > abs(road_theta)) ? theta : road_theta;
    theta = (float) theta / CV_PI * 180; // convert to degree
    theta = theta * 10;

    // TODO: tang, giam theo thoi gian
    int velocity = this->car->getCurrentVelocity();
    if (abs(road_theta - cur_road_theta) < 5) {
        velocity += 2;
    }
    else {
        velocity -= 2;
    }

    // velocity in [20, 45]
    if (velocity > 45) {
        velocity = 45;
    }
    else if (velocity < 20) {
        velocity = 20;
    }

    /////////// RUN //////////////////
    if (this->car->update(direction, velocity, theta, this->car->getCurrentState()) < 0) {
        std::cerr << "Car hardware connection corrupted." << std::endl;
        return -1;
    }

    this->cur_road_theta = road_theta;
    return 0;
}

void Driver::release() {
    this->car->release();
    delete this->car;
    this->car = nullptr;
}
