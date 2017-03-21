#ifndef __SB_FRAME_INFO_H__
#define __SB_FRAME_INFO_H__

#include "EdgeDetector.h"
#include "JoinerSplitter.h"
#include "LineDetector.h"

namespace sb
{
class FrameInfo
{
private:
	cv::Mat _colorFrame;

	std::vector<sb::Line> _lines;

	std::vector<sb::Section> _sections;
public:
	FrameInfo();

	void create( const cv::Mat& colorFrame,
	             const sb::JoinerSplitter& spliter,
	             const sb::EdgeDetector& edgeDetector,
	             const sb::LineDetector& lineDetector );

	const cv::Mat& getColorFrame() const;

	const std::vector<sb::Line>& getLines() const;

	std::vector<sb::Line>& getLines();

	const std::vector<sb::Section>& getSections() const;

	std::vector<sb::Section>& getSections();
};
}

#endif //!__SB_FRAME_INFO_H__
