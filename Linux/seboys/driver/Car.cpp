#include "driver/Car.h"

int sb::Car::init() {
    ////////  Init PCA9685 driver   ///////////////////////////////////////////
    pca9685 = new PCA9685();
    api_pwm_pca9685_init(pca9685);

    // Start Car Engine
    this->currentState = DIR_BRAKE;
    if (pca9685->error >= 0) {
        api_pwm_set_control(pca9685, this->currentState, 0, 0.0, this->currentState);
    }

    /////////  Init UART here   ///////////////////////////////////////////////
    cport_nr = api_uart_open();

    if (cport_nr == -1) {
        cerr << "Error: Canot Open ComPort";
        return -1;
    }

    return 0;
}

int sb::Car::update(int &direction, int &velocity, double &steeringAngle) {
    //// Check PCA9685 driver ////////////////////////////////////////////
    if (pca9685->error < 0) {
        cout << endl << "Error: PWM driver" << endl << flush;
        return -1;
    }

    if (velocity >= 0) {
        sprintf(buf_send, "f%d\n", velocity);
    } else {
        sprintf(buf_send, "b%d\n", -velocity);
    }

    api_uart_write(cport_nr, buf_send);
    api_pwm_set_control(pca9685, direction, velocity, steeringAngle, currentState);

    this->currentState = direction;

    return 0;
}

int sb::Car::getCurrentState() {
    return this->currentState;
}

int sb::Car::release() {
    api_pwm_pca9685_release(pca9685);
    delete this->pca9685;
    this->pca9685 = nullptr;
    return 0;
}
