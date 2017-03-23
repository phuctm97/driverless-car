#ifndef __SB_CAR_H__
#define __SB_CAR_H__

#include <iostream>
#include <cmath>
#include <cstring>

/*

#include "api_i2c_pwm.h"
#include "api_uart.h"

*/

namespace sb
{
class Car
{
private:
	/*
	
	int cport_nr;
	PCA9685* pca9685;

	*/

public:
	Car() {}

	int init( int& direction, int& velocity, int& steeringAngle, int& currentState );

	int update( int& direction, int& velocity, int& steeringAngle, int& currentState );

	void release( int& direction, int& velocity, int& steeringAngle, int& currentState );
};
}

#endif //!__SB_CAR_H__
