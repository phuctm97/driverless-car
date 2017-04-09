#include "LanePart.h"

sb::LanePart::LanePart( const cv::Point2d& _position, double _angle, double _width, double _length )
	: position( _position ), angle( _angle ), width( _width )
{
	if( _length > 0 )
		calculateVertices( _length );
}

bool sb::LanePart::operator==( const sb::LanePart& other ) const
{
	double angle_diff = abs( angle - other.angle );
	double width_diff = abs( width - other.width );

	cv::Point2d pos_diff = position - other.position;
	double pos_distance = std::sqrt( pos_diff.x * pos_diff.x + pos_diff.y * pos_diff.y );

	// **argument to be set
	return angle_diff <= 2 && width_diff <= 2 && pos_distance <= 2;
}

void sb::LanePart::calculateVertices( double _length )
{
	vertices[0] = position;

	cv::Point2d horizontal_vec, vertical_vec;

	// calculate vectors 
	{
		if( angle == 90 )
			vertical_vec = cv::Point2d( 0, 1 );
		else if( angle > 90 )
			vertical_vec = cv::Point2d( 1, tan( -angle * CV_2PI / 360 ) );
		else
			vertical_vec = -cv::Point2d( 1, tan( -angle * CV_2PI / 360 ) );

		double vertical_vec_length = std::sqrt( vertical_vec.x * vertical_vec.x +
																						vertical_vec.y * vertical_vec.y );
		vertical_vec = cv::Point2d( vertical_vec.x / vertical_vec_length,
																vertical_vec.y / vertical_vec_length );
		horizontal_vec = cv::Point2d( vertical_vec.y, -vertical_vec.x );
	}

	vertices[1] = vertices[0] + vertical_vec * _length;
	vertices[2] = vertices[1] + horizontal_vec * width;
	vertices[3] = vertices[0] + horizontal_vec * width;
}