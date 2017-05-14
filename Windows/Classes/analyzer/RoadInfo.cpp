#include "RoadInfo.h"

void sb::create( sb::RoadInfo* roadInfo, Params* params )
{
	roadInfo->leftKnots.clear();
	roadInfo->rightKnots.clear();
}

void sb::release( sb::RoadInfo* roadInfo )
{
	roadInfo->leftKnots.clear();
	roadInfo->rightKnots.clear();
}

void sb::RoadInfo::read( const cv::FileNode& node )
{
	node["Target"] >> target;
	node["LeftKnots"] >> leftKnots;
	node["RightKnots"] >> rightKnots;
}

void sb::RoadInfo::write( cv::FileStorage& fs ) const
{
	fs
			<< "{"
			<< "Target" << target
			<< "LeftKnots" << leftKnots
			<< "RightKnots" << rightKnots
			<< "}";
}

void sb::write( cv::FileStorage& fs, const std::string&, const sb::RoadInfo& data )
{
	data.write( fs );
}

<<<<<<< HEAD
void sb::read( const cv::FileNode& node, sb::RoadInfo& data, const sb::RoadInfo& defaultData )
{
	if ( node.empty() ) data = defaultData;
=======
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
>>>>>>> master
	else data.read( node );
}
