#include "Params.h"

void sb::Params::load( const cv::String& yamlFileName )
{
	cv::FileStorage fs( yamlFileName, cv::FileStorage::READ );

	fs[PARAMS_YAML_FIELD_NAME] >> *this;

	fs.release();
}

void sb::Params::save( const cv::String& yamlFileName ) const
{
	cv::FileStorage fs( yamlFileName, cv::FileStorage::WRITE );

	fs << PARAMS_YAML_FIELD_NAME << *this;

	fs.release();
}

void sb::Params::write( cv::FileStorage& fs ) const
{
	fs
			<< "{"

			<< "COLOR_FRAME_SIZE" << COLOR_FRAME_SIZE
			<< "DEPTH_FRAME_SIZE" << DEPTH_FRAME_SIZE

			<< "CROP_SIZE_WIDTH" << CROP_SIZE_WIDTH
			<< "CROP_SIZE_HEIGHT" << CROP_SIZE_HEIGHT

			<< "SEPERATE_ROWS" << SEPERATE_ROWS

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

			<< "WARP_SRC_QUAD" << std::vector<cv::Point2f>( WARP_SRC_QUAD, WARP_SRC_QUAD + 4 )
			<< "WARP_DST_QUAD" << std::vector<cv::Point2f>( WARP_DST_QUAD, WARP_DST_QUAD + 4 )

			<< "INITIAL_POSITION_OF_LEFT_LANE" << INITIAL_POSITION_OF_LEFT_LANE
			<< "INITIAL_POSITION_OF_RIGHT_LANE" << INITIAL_POSITION_OF_RIGHT_LANE
			<< "INITIAL_ROTATION_OF_LANES" << INITIAL_ROTATION_OF_LANES

			<< "}";
}

void sb::Params::read( const cv::FileNode& node )
{
	node["COLOR_FRAME_SIZE"] >> COLOR_FRAME_SIZE;
	node["DEPTH_FRAME_SIZE"] >> DEPTH_FRAME_SIZE;

	node["CROP_SIZE_WIDTH"] >> CROP_SIZE_WIDTH;
	node["CROP_SIZE_HEIGHT"] >> CROP_SIZE_HEIGHT;

	node["SEPERATE_ROWS"] >> SEPERATE_ROWS;

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

	std::vector<cv::Point2f> srcQuadVec;
	node["WARP_SRC_QUAD"] >> srcQuadVec;
	std::copy( srcQuadVec.begin(), srcQuadVec.end(), WARP_SRC_QUAD );

	std::vector<cv::Point2f> dstQuadVec;
	node["WARP_DST_QUAD"] >> dstQuadVec;
	std::copy( dstQuadVec.begin(), dstQuadVec.end(), WARP_DST_QUAD );

	node["INITIAL_POSITION_OF_LEFT_LANE"] >> INITIAL_POSITION_OF_LEFT_LANE;
	node["INITIAL_POSITION_OF_RIGHT_LANE"] >> INITIAL_POSITION_OF_RIGHT_LANE;
	node["INITIAL_ROTATION_OF_LANES"] >> INITIAL_ROTATION_OF_LANES;
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
