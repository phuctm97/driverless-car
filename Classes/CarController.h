#pragma once

class CarController
{
public:
	CarController( int speed ) {}

	void changeSpeed( int speed );

	void changeTheta( int theta );

	void start();

	void pause();

	void stop();

	void update();
};
