#include "RoadInfo.h"

void sb::RoadInfo::create( const Params& params )
{
	_leftKnots.clear();
	_rightKnots.clear();
}

void sb::RoadInfo::setObstacleBoxes( const std::vector<cv::Rect2d>& obstacleBoxes ) { _obstacleBoxes = obstacleBoxes; }

const std::vector<cv::Rect2d>& sb::RoadInfo::getObstacleBoxes() const { return _obstacleBoxes; }

void sb::RoadInfo::setLeftKnots( const std::vector<cv::Point2d>& leftKnots ) { _leftKnots = leftKnots; }

const std::vector<cv::Point2d>& sb::RoadInfo::getLeftKnots() const { return _leftKnots; }

void sb::RoadInfo::setRightKnots( const std::vector<cv::Point2d>& rightKnots ) { _rightKnots = rightKnots; }

const std::vector<cv::Point2d>& sb::RoadInfo::getRightKnots() const { return _rightKnots; }

void sb::RoadInfo::setReliability( float reliability ) { _reliability = reliability; }

float sb::RoadInfo::getReliability() const { return _reliability; }

void sb::RoadInfo::read( const cv::FileNode& node )
{
	node["LeftKnots"] >> _leftKnots;
	node["RightKnots"] >> _rightKnots;
}

void sb::RoadInfo::write( cv::FileStorage& fs ) const
{
	fs
			<< "{"
			<< "LeftKnots" << _leftKnots
			<< "RightKnots" << _rightKnots
			<< "}";
}

void sb::write( cv::FileStorage& fs, const std::string&, const sb::RoadInfo& data )
{
	data.write( fs );
}

void sb::read( const cv::FileNode& node, sb::RoadInfo& data, const sb::RoadInfo& defaultData )
{
	if( node.empty() ) data = defaultData;
	else data.read( node );
}
