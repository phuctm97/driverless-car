#ifndef __SB_CAR_H__
#define __SB_CAR_H__

#include "api_i2c_pwm.h"
#include "api_uart.h"
#include <cmath>
#include <iostream>

class Car {
private:
    int cport_nr; // port id of uart.
    char buf_send[BUFF_SIZE]; // buffer to store and recive controller messages.

    PCA9685 *pca9685; // driver

    int currentState;
    int currentVelocity;
    double currentTheta;

public:
    Car() {}

    int init();
    int update(int &direction, int &velocity, double &steeringAngle);
    int release();

    int getCurrentState();
    int getCurrentVelocity();
    double getCurrentTheta();
};


#endif //!__SB_CAR_H__
