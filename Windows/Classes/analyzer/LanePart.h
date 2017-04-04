#ifndef __SB_LANE_PART_H__
#define __SB_LANE_PART_H__

#include "../Include.h"

namespace sb
{
struct LanePart
{
	cv::Point2d vertices[4];

	cv::Point2d position;

	double width;

	double angle;

	LanePart()
		: width( 0 ), angle( 0 ), position( 0, 0 ) {}

	LanePart( const cv::Point2d& _position, double _angle, double _width, double _length = -1 );

	bool operator==( const sb::LanePart& other ) const;

	void calculateVertices( double _length );
};
}

#endif //!__SB_LANE_PART_H__