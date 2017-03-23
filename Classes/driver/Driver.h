#ifndef __SB_DRIVER_H__
#define __SB_DRIVER_H__

#include "../Params.h"
#include "../analyzer/RoadInfo.h"
#include "Car.h"

#define DIR_FORWARD 0
#define DIR_BRAKE 0

namespace sb
{
class Driver
{
private:
	sb::Car _car;

	int direction;

	int vel_add;

	int velocity;

	int theta; // steering angle

	int current_direction; // CURRENT DIRECTION

	float current_thera_road; // xu huong duong hien tai

public:
	Driver()
		:
		direction( DIR_FORWARD ),
		velocity( 30 ),
		theta( 0 ),
		current_direction( DIR_BRAKE ),
		current_thera_road( 0 ) {}

	int init( const sb::Params& params );

	int drive( const sb::RoadInfo& roadInfo );

	void release();
};
}

#endif //!__SB_DRIVER_H__
