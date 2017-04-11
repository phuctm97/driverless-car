#include "RoadInfo.h"

void sb::create( sb::RoadInfo* roadInfo, Params* params )
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

void sb::read( const cv::FileNode& node, sb::RoadInfo& data, const sb::RoadInfo& defaultData )
{
	if ( node.empty() ) data = defaultData;
	else data.read( node );
}
