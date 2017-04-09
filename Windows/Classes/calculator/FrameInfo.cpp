#include "FrameInfo.h"

void sb::FrameInfo::create( const sb::Params& params )
{
}

const cv::Mat& sb::FrameInfo::getColorImage() const { return _colorImage; }

void sb::FrameInfo::setColorImage( const cv::Mat& colorImage ) { _colorImage = colorImage; }

const std::vector<sb::Section>& sb::FrameInfo::getImageSections() const { return _imageSections; }

void sb::FrameInfo::setImageSections( const std::vector<sb::Section>& imageSections ) { _imageSections = imageSections; }

const cv::Mat& sb::FrameInfo::getEdgesImage() const { return _edgesImage; }

void sb::FrameInfo::setEdgesImage( const cv::Mat& edgesImage ) { _edgesImage = edgesImage; }

