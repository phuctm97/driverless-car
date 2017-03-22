#include "FrameInfo.h"

sb::FrameInfo::FrameInfo() {}

int sb::FrameInfo::create( const cv::Mat& colorImage,
                           const cv::Mat& depthImage,
                           const sb::Formatter& formatter,
                           const sb::EdgeDetector& edgeDetector,
                           const sb::LineDetector& lineDetector )
{
	// 1) reference to new frame
	_colorImage.release();
	if ( formatter.crop( colorImage, _colorImage ) < 0 ) {
		std::cerr << "Crop image failed." << std::endl;
		return -1;
	}

	_depthImage.release();
	_depthImage = depthImage;

	// 2) generate edges-frame
	cv::Mat edgesFrame;

	cv::cvtColor( _colorImage, edgesFrame, cv::COLOR_BGR2GRAY );

	edgeDetector.apply( edgesFrame );

	// 3) generate lines in whole frame
	lineDetector.apply( edgesFrame, _lines );

	return 0;
}

const cv::Mat& sb::FrameInfo::getColorImage() const { return _colorImage; }

const cv::Mat& sb::FrameInfo::getDepthImage() const { return _depthImage; }

const std::vector<sb::LineInfo>& sb::FrameInfo::getLines() const { return _lines; }
