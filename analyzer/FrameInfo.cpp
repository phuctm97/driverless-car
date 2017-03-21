#include "FrameInfo.h"

sb::FrameInfo::FrameInfo() {}

void sb::FrameInfo::create( const cv::Mat& colorFrame,
                            const sb::JoinerSplitter& spliter,
                            const sb::EdgeDetector& edgeDetector,
                            const sb::LineDetector& lineDetector )
{
	// 1) reference to new frame
	_colorFrame.release();
	_colorFrame = colorFrame;

	// 2) generate edges-frame
	cv::Mat edgesFrame;

	cv::cvtColor( colorFrame, edgesFrame, cv::COLOR_BGR2GRAY );

	edgeDetector.apply( edgesFrame );

	// 3) generate lines in whole frame
	lineDetector.apply( edgesFrame, _lines );

	// 4) generate sections
	spliter.splitImageToSections( edgesFrame, _sections );

	// 5) generate lines in each section
	for ( sb::Section& section : _sections ) {
		lineDetector.apply( section );
	}
}

const cv::Mat& sb::FrameInfo::getColorFrame() const { return _colorFrame; }

const std::vector<sb::Line>& sb::FrameInfo::getLines() const { return _lines; }

std::vector<sb::Line>& sb::FrameInfo::getLines() { return _lines; }

const std::vector<sb::Section>& sb::FrameInfo::getSections() const { return _sections; }

std::vector<sb::Section>& sb::FrameInfo::getSections() { return _sections; }
