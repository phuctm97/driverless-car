//
// Created by Pham Huu Danh on 3/14/17.
//
#include "SeboyDriver.h"

using namespace cv;

int SeboyDriver::init() {

    ////////  Init PCA9685 driver   ///////////////////////////////////////////
    pca9685 = new PCA9685();
    api_pwm_pca9685_init(pca9685);

    if (pca9685->error >= 0)
        api_pwm_set_control(pca9685, direction, velocity, theta, current_direction);

    /////////  Init UART here   ///////////////////////////////////////////////
    cport_nr = api_uart_open();

    if (cport_nr == -1) {
        cerr << "Error: Canot Open ComPort";
        return -1;
    }
}

int SeboyDriver::update(float bottom, float right, float top, float left, float reliability) {

    //// Check PCA9685 driver ////////////////////////////////////////////
    if (pca9685->error < 0) {
        cout << endl << "Error: PWM driver" << endl << flush;
        return -1;
    }

    float h_div_w = 0.25f * VIDEO_FRAME_HEIGHT / VIDEO_FRAME_WIDTH; // ti le 1 phan 4 height tren width

    float mid_1 = (bottom + right) / 2.0f;
    float mid_2 = (left + top) / 2.0f;

    float theta_road = atan((mid_2 - mid_1) / h_div_w); // do lech cua duong
    float theta_car = atan(mid_2 / h_div_w); // do lech cua xe so voi tam duong sap den

    float d = abs(mid_1); // do lech cua xe so voi tam duong hien tai

    // TODO: tang, giam toc do theo delta_theta_road
    float delta_theta_road = abs(theta_road) - abs(current_thera_road);
    if (delta_theta_road < 0.0f) { // duong co xu huong thang
        this->vel_add += 2; // tang toc them 2km/h
    } else { // duong co xu huong cong
        this->vel_add -= 2; // giam toc di 2km/h
    }

    // TODO: stop & xoay xe
    if (d > 0.3) { // dang di lech huong qua nhieu
        this->vel_add = -10;
    }

    theta_car = (float) (theta_car * 180 / CV_PI); // rad to degree
    if (abs(theta_car) <= 20) {
        this->theta = theta_car * 10;
    } else {
        this->theta = theta_car / abs(theta_car) * 200;
    }

    int vel = this->velocity + this->vel_add;

    /////////// RUN //////////////////
    api_uart_write(this->cport_nr, this->buf_send);
    cout << vel << endl;
    api_pwm_set_control(this->pca9685, this->direction, vel, this->theta, this->current_direction);
    this->current_direction = this->direction;

    return 0;
}