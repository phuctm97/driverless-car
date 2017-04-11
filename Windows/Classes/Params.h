#ifndef __SB_PARAMS_H__
#define __SB_PARAMS_H__

#include "Include.h"

#define PARAMS_YAML_FIELD_NAME "Params"


namespace sb
{
struct Params
{
	cv::Size COLOR_FRAME_SIZE = cv::Size( 640, 480 );
	cv::Rect CROP_BOX = cv::Rect( 0, 0, 0, 0 );
	double CONVERT_COORD_COEF = 1.0 / 40;

	double MIN_LANE_WIDTH = 2;
	double MAX_LANE_WIDTH = 15;
	double MIN_ROAD_WIDTH = 60;
	double MAX_ROAD_WIDTH = 100;

	int EDGE_DETECTOR_KERNEL_SIZE = 3;
	double EDGE_DETECTOR_LOW_THRESH = 100;
	double EDGE_DETECTOR_HIGH_THRESH = 200;

	double BINARIZE_THRESH = 200;
	double BINARIZE_MAX_VALUE = 255;

	double HOUGH_LINES_P_RHO = 1;
	double HOUGH_LINES_P_THETA = CV_2PI / 360;
	int HOUGH_LINES_P_THRESHOLD = 10;
	double HOUGH_LINES_P_MIN_LINE_LENGTH = 2;
	double HOUGH_LINES_P_MAX_LINE_GAP = 7;

	cv::Point2f WARP_SRC_QUAD[4];
	cv::Point2f WARP_DST_QUAD[4];

	int MIN_STEERING_ANGLE = -200;
	int MAX_STEERING_ANGLE = 200;
	int MAX_VELOCITY = 45;
	int INITIAL_VELOCITY = 10;

	void read( const cv::FileNode& node );

	void write( cv::FileStorage& fs ) const;
};

void load( sb::Params* params, const cv::String& yamlFileName );

void save( sb::Params* params, const cv::String& yamlFileName );

void write( cv::FileStorage& fs, const std::string&, const sb::Params& data );

void read( const cv::FileNode& node, sb::Params& data, const sb::Params& defaultData );
}

#endif //!__SB_PARAMS_H__
