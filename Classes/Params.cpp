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

			<< "CROP_SIZE_WIDTH" << CROP_SIZE_WIDTH
			<< "CROP_SIZE_HEIGHT" << CROP_SIZE_HEIGHT

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

			<< "}";
}

void sb::Params::read( const cv::FileNode& node )
{
	node["CROP_SIZE_WIDTH"] >> CROP_SIZE_WIDTH;
	node["CROP_SIZE_HEIGHT"] >> CROP_SIZE_HEIGHT;

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
