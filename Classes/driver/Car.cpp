#include "Car.h"

int sb::Car::init( int& direction, int& velocity, int& steeringAngle, int& currentState )
{
	////////  Init PCA9685 driver   ///////////////////////////////////////////
	
	/*

	 pca9685 = new PCA9685();
	 api_pwm_pca9685_init( pca9685 );

	///////  Init UART here   ///////////////////////////////////////////////
	
	 cport_nr = api_uart_open();
	 if( cport_nr < -1 ) {
		 std::cerr << "Init UART failed: Canot Open ComPort" << std::endl;
		 return -1;
	 }

	///////// Initial run //////////////////////////

	 update(direction, velocity, steeringAngle, currentState);

	*/

	return 0;
}

int sb::Car::update( int& direction, int& velocity, int& steeringAngle, int& currentState )
{
	/*
	
	 if (pca9685->error < 0) {
       std::cerr << "PWM driver check failed." << std::endl;
       return -1;
   }

	char buf_send[BUFF_SIZE];

	if( velocity >= 0 ) {
		sprintf( buf_send, "f%d\n", velocity );
	}
	else {
		sprintf( buf_send, "b%d\n", -velocity );
	}

	api_uart_write( cport_nr, buf_send );

	api_pwm_set_control( pca9685, direction, velocity, steeringAngle, currentState );

	*/

	return 0;
}

void sb::Car::release( int& direction, int& velocity, int& steeringAngle, int& currentState )
{
	/*

	velocity = 0;
	
	steeringAngle = 0;

	update(direction, velocity, steeringAngle, currentState);

	api_pwm_pca9685_release( pca9685 );

	*/
}
