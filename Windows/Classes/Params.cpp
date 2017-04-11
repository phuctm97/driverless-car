#include "Params.h"

void sb::load( sb::Params* params, const cv::String& yamlFileName )
{
	cv::FileStorage fs( yamlFileName, cv::FileStorage::READ );

	fs[PARAMS_YAML_FIELD_NAME] >> *params;

	fs.release();
}

void sb::save( sb::Params* params, const cv::String& yamlFileName ) 
{
	cv::FileStorage fs( yamlFileName, cv::FileStorage::WRITE );

	fs << PARAMS_YAML_FIELD_NAME << *params;

	fs.release();
}

void sb::Params::write( cv::FileStorage& fs ) const
{
	fs
			<< "{"

			<< "Group_1" << "<Image manipulating fields>"
			<< "COLOR_FRAME_SIZE" << COLOR_FRAME_SIZE
			<< "CROP_BOX" << CROP_BOX
			<< "WARP_SRC_QUAD" << std::vector<cv::Point2f>( WARP_SRC_QUAD, WARP_SRC_QUAD + 4 )
			<< "WARP_DST_QUAD" << std::vector<cv::Point2f>( WARP_DST_QUAD, WARP_DST_QUAD + 4 )

			<< "Group_2" << "<Line detecting fields>"
			<< "EDGE_DETECTOR_KERNEL_SIZE" << EDGE_DETECTOR_KERNEL_SIZE
			<< "EDGE_DETECTOR_LOW_THRESH" << EDGE_DETECTOR_LOW_THRESH
			<< "EDGE_DETECTOR_HIGH_THRESH" << EDGE_DETECTOR_HIGH_THRESH
			<< "BINARIZE_THRESH" << BINARIZE_THRESH
			<< "BINARIZE_MAX_VALUE" << BINARIZE_MAX_VALUE
			<< "HOUGH_LINES_P_RHO" << HOUGH_LINES_P_RHO
			<< "HOUGH_LINES_P_THETA" << HOUGH_LINES_P_THETA
			<< "HOUGH_LINES_P_THRESHOLD" << HOUGH_LINES_P_THRESHOLD
			<< "HOUGH_LINES_P_MIN_LINE_LENGTH" << HOUGH_LINES_P_MIN_LINE_LENGTH
			<< "HOUGH_LINES_P_MAX_LINE_GAP" << HOUGH_LINES_P_MAX_LINE_GAP

			<< "Group_3" << "<Parameters for real space processing>"
			<< "CONVERT_COORD_COEF" << CONVERT_COORD_COEF
			<< "MIN_LANE_WIDTH" << MIN_LANE_WIDTH
			<< "MAX_LANE_WIDTH" << MAX_LANE_WIDTH
			<< "MIN_ROAD_WIDTH" << MIN_ROAD_WIDTH
			<< "MAX_ROAD_WIDTH" << MAX_ROAD_WIDTH

			<< "Group_4" << "<Parameters for vehicle>"
			<< "MIN_STEERING_ANGLE" << MIN_STEERING_ANGLE
			<< "MAX_STEERING_ANGLE" << MAX_STEERING_ANGLE
			<< "MAX_VELOCITY" << MAX_VELOCITY
			<< "INITIAL_VELOCITY" << INITIAL_VELOCITY

			<< "}";
}

void sb::Params::read( const cv::FileNode& node )
{
	node["COLOR_FRAME_SIZE"] >> COLOR_FRAME_SIZE;
	node["CROP_BOX"] >> CROP_BOX;

	std::vector<cv::Point2f> srcQuadVec;
	node["WARP_SRC_QUAD"] >> srcQuadVec;
	std::copy( srcQuadVec.begin(), srcQuadVec.end(), WARP_SRC_QUAD );

	std::vector<cv::Point2f> dstQuadVec;
	node["WARP_DST_QUAD"] >> dstQuadVec;
	std::copy( dstQuadVec.begin(), dstQuadVec.end(), WARP_DST_QUAD );

	node["EDGE_DETECTOR_KERNEL_SIZE"] >> EDGE_DETECTOR_KERNEL_SIZE;
	node["EDGE_DETECTOR_LOW_THRESH"] >> EDGE_DETECTOR_LOW_THRESH;
	node["EDGE_DETECTOR_HIGH_THRESH"] >> EDGE_DETECTOR_HIGH_THRESH;
	node["BINARIZE_THRESH"] >> BINARIZE_THRESH;
	node["BINARIZE_MAX_VALUE"] >> BINARIZE_MAX_VALUE;
	node["HOUGH_LINES_P_RHO"] >> HOUGH_LINES_P_RHO;
	node["HOUGH_LINES_P_THETA"] >> HOUGH_LINES_P_THETA;
	node["HOUGH_LINES_P_THRESHOLD"] >> HOUGH_LINES_P_THRESHOLD;
	node["HOUGH_LINES_P_MIN_LINE_LENGTH"] >> HOUGH_LINES_P_MIN_LINE_LENGTH;
	node["HOUGH_LINES_P_MAX_LINE_GAP"] >> HOUGH_LINES_P_MAX_LINE_GAP;

	node["CONVERT_COORD_COEF"] >> CONVERT_COORD_COEF;
	node["MIN_LANE_WIDTH"] >> MIN_LANE_WIDTH;
	node["MAX_LANE_WIDTH"] >> MAX_LANE_WIDTH;
	node["MIN_ROAD_WIDTH"] >> MIN_ROAD_WIDTH;
	node["MAX_ROAD_WIDTH"] >> MAX_ROAD_WIDTH;

	node["MIN_STEERING_ANGLE"] >> MIN_STEERING_ANGLE;
	node["MAX_STEERING_ANGLE"] >> MAX_STEERING_ANGLE;
	node["MAX_VELOCITY"] >> MAX_VELOCITY;
	node["INITIAL_VELOCITY"] >> INITIAL_VELOCITY;
}

void sb::write( cv::FileStorage& fs, const std::string&, const sb::Params& data )
{
	data.write( fs );
}

void sb::read( const cv::FileNode& node, sb::Params& data, const sb::Params& defaultData )
{
	if ( node.empty() ) data = defaultData;
	else data.read( node );
}
