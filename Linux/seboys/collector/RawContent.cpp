#include "RawContent.h"

const cv::Mat& RawContent::getColorImage() const { return _colorImage; }

void RawContent::setColorImage( const cv::Mat& colorImage ) { _colorImage = colorImage; }

const cv::Mat& RawContent::getDepthImage() const { return _depthImage; }

void RawContent::setDepthImage( const cv::Mat& depthImage ) { _depthImage = depthImage; }
