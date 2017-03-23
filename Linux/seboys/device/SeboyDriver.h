//
// Created by Pham Huu Danh on 3/14/17.
//

#ifndef SECAR_SEBOYDRIVER_H
#define SECAR_SEBOYDRIVER_H

#include "api_kinect_cv.h"
// api_kinect_cv.h: manipulate openNI2, kinect, depthMap and object detection
#include "api_i2c_pwm.h"
#include "api_uart.h"
#include <cmath>
#include <iostream>

#define READ_WAIT_TIMEOUT 2000 //2000ms
#define VIDEO_FRAME_WIDTH 640
#define VIDEO_FRAME_HEIGHT 480

class SeboyDriver {
private:
    int cport_nr; // port id of uart.
    char buf_send[BUFF_SIZE]; // buffer to store and recive controller messages.
    PCA9685 *pca9685; // driver

    int direction = DIR_FORWARD;

    int vel_add = 0;

    int velocity = 30;
    double theta = 0.0d; // steering angle

    int current_direction = DIR_BRAKE; // CURRENT DIRECTION

    float current_thera_road = 0.0f; // xu huong duong hien tai

public:

    int init();

    int update(float bottom, float right, float top, float left, float reliability);
};

#endif //SECAR_SEBOYDRIVER_H
