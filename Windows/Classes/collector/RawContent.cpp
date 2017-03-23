#include "RawContent.h"

void sb::RawContent::create( const sb::Params & params )
{
	_colorImage = cv::Mat::zeros( params.COLOR_FRAME_SIZE, CV_8UC3 );
	_depthImage = cv::Mat::zeros( params.DEPTH_FRAME_SIZE, CV_8U );
}

const cv::Mat& sb::RawContent::getColorImage() const { return _colorImage; }

void sb::RawContent::setColorImage( const cv::Mat& colorImage ) { _colorImage = colorImage; }

const cv::Mat& sb::RawContent::getDepthImage() const { return _depthImage; }

void sb::RawContent::setDepthImage( const cv::Mat& depthImage ) { _depthImage = depthImage; }
