#include "FrameInfo.h"

void sb::FrameInfo::create( const sb::Params& params )
{
}

const cv::Mat& sb::FrameInfo::getColorImage() const { return _colorImage; }

void sb::FrameInfo::setColorImage( const cv::Mat& colorImage ) { _colorImage = colorImage; }

const std::vector<sb::LineInfo>& sb::FrameInfo::getImageLineInfos() const { return _imageLineInfos; }

void sb::FrameInfo::setImageLineInfos( const std::vector<sb::LineInfo>& lineInfos ) { _imageLineInfos = lineInfos; }

const std::vector<sb::LineInfo>& sb::FrameInfo::getRealLineInfos() const { return _realLineInfos; }

void sb::FrameInfo::setRealLineInfos( const std::vector<sb::LineInfo>& warpedLines ) { _realLineInfos = warpedLines; }

