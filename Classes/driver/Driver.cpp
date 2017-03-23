#include "Driver.h"

int sb::Driver::init( const sb::Params& params )
{
	if( _car.init( direction, velocity, theta, current_direction) < 0 ) {
		std::cerr << "Car init failed." << std::endl;
		return -1;
	}


	return 0;
}

int sb::Driver::drive( const sb::RoadInfo& roadInfo )
{
	/*
	
	float h_div_w = 0.25f * VIDEO_FRAME_HEIGHT / VIDEO_FRAME_WIDTH; // ti le 1 phan 4 height tren width

	float mid_1 = (bottom + right) / 2.0f;
	float mid_2 = (left + top) / 2.0f;

	float theta_road = atan( (mid_2 - mid_1) / h_div_w ); // do lech cua duong
	float theta_car = atan( mid_2 / h_div_w ); // do lech cua xe so voi tam duong sap den

	float d = abs( mid_1 ); // do lech cua xe so voi tam duong hien tai

													// TODO: tang, giam toc do theo delta_theta_road
	float delta_theta_road = abs( theta_road ) - abs( current_thera_road );
	if( delta_theta_road < 0.0f ) { // duong co xu huong thang
		this->vel_add += 2; // tang toc them 2km/h
	}
	else { // duong co xu huong cong
		this->vel_add -= 2; // giam toc di 2km/h
	}

	// TODO: stop & xoay xe
	if( d > 0.3 ) { // dang di lech huong qua nhieu
		this->vel_add = -10;
	}

	theta_car = (float) (theta_car * 180 / CV_PI); // rad to degree
	if( abs( theta_car ) <= 20 ) {
		this->theta = theta_car * 10;
	}
	else {
		this->theta = theta_car / abs( theta_car ) * 200;
	}

	int vel = this->velocity + this->vel_add;

	/////////// RUN //////////////////
	if(_car.update( this->direction, vel, this->theta, this->current_direction ) < 0 ) {
		std::cerr << "Car hardware connection corrupted." << std::endl;
		return -1;
	}
	this->current_direction = this->direction;

	*/

	return 0;
}

void sb::Driver::release()
{
	_car.release( direction, velocity, theta, current_direction );

}
