﻿#ifndef __SB_PARAMS_H__
#define __SB_PARAMS_H__

#include "Include.h"

#define PARAMS_YAML_FIELD_NAME "Params"

namespace sb
{
struct Params
{
	float CROP_SIZE_WIDTH = 0.8f;
	float CROP_SIZE_HEIGHT = 0.25f;

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

	void load( const cv::String& yamlFileName );
	
	void read( const cv::FileNode& node );

	void save( const cv::String& yamlFileName ) const;

	void write( cv::FileStorage& fs ) const;
};

void write( cv::FileStorage& fs, const std::string&, const sb::Params& data );

void read( const cv::FileNode& node, sb::Params& data, const sb::Params& defaultData );
}

#endif //!__SB_PARAMS_H__