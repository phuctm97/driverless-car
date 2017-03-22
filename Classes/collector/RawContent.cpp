#include "RawContent.h"

const cv::Mat& sb::RawContent::getColorImage() const { return _colorImage; }

void sb::RawContent::setColorImage( const cv::Mat& colorImage ) { _colorImage = colorImage; }

const cv::Mat& sb::RawContent::getDepthImage() const { return _depthImage; }

void sb::RawContent::setDepthImage( const cv::Mat& depthImage ) { _depthImage = depthImage; }
