#include "RoadInfo.h"

void sb::RoadInfo::create( const Params& params )
{
	_leftKnots.clear();
	_rightKnots.clear();

	_leftKnots.assign( params.SEPERATE_ROWS.size(), cv::Point2d() );
	_rightKnots.assign( params.SEPERATE_ROWS.size(), cv::Point2d() );

	_leftKnots[0] = cv::Point2d( params.INITIAL_POSITION_OF_LEFT_LANE, params.SEPERATE_ROWS[0] );
	_rightKnots[0] = cv::Point2d( params.INITIAL_POSITION_OF_RIGHT_LANE, params.SEPERATE_ROWS[0] );

	const int N_SECTIONS = static_cast<int>(params.INITIAL_ROTATION_OF_LANES.size());
	for ( int i = 0; i < N_SECTIONS; i++ ) {
		const int upperRow = params.SEPERATE_ROWS[i + 1];
		const int lowerRow = params.SEPERATE_ROWS[i];

		const sb::Line upperLine( cv::Point2d( 0, upperRow ), cv::Point2d( 1, upperRow ) );

		sb::Line line;

		line = sb::Line( params.INITIAL_ROTATION_OF_LANES[i], _leftKnots[i] );

		sb::Line::findIntersection( line, upperLine, _leftKnots[i + 1] );

		line = sb::Line( params.INITIAL_ROTATION_OF_LANES[i], _rightKnots[i] );

		sb::Line::findIntersection( line, upperLine, _rightKnots[i + 1] );
	}
}

void sb::RoadInfo::setObstacleBoxes( const std::vector<cv::Rect2d>& obstacleBoxes ) { _obstacleBoxes = obstacleBoxes; }

const std::vector<cv::Rect2d>& sb::RoadInfo::getObstacleBoxes() const { return _obstacleBoxes; }

void sb::RoadInfo::setLeftKnots( const std::vector<cv::Point2d>& leftKnots ) { _leftKnots = leftKnots; }

const std::vector<cv::Point2d>& sb::RoadInfo::getLeftKnots() const { return _leftKnots; }

void sb::RoadInfo::setRightKnots( const std::vector<cv::Point2d>& rightKnots ) { _rightKnots = rightKnots; }

const std::vector<cv::Point2d>& sb::RoadInfo::getRightKnots() const { return _rightKnots; }

void sb::RoadInfo::setReliability( float reliability ) { _reliability = reliability; }

float sb::RoadInfo::getReliability() const { return _reliability; }

