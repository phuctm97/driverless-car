#ifndef __SB_FRAME_INFO_H__
#define __SB_FRAME_INFO_H__

#include "Formatter.h"
#include "EdgeDetector.h"
#include "LineDetector.h"
#include "../Params.h"

namespace sb
{

class FrameInfo
{
private:
	cv::Point2d _topLeftPoint;

	cv::Mat _colorImage;

	cv::Mat _depthImage;

	std::vector<sb::LineInfo> _lines;

	std::vector<sb::LineInfo> _warpedLines;

	std::vector<sb::SectionInfo> _sections;

public:
	FrameInfo();

	void create( const sb::Params& params );

	int create( const cv::Mat& colorImage,
	            const cv::Mat& depthImage,
	            const sb::Formatter& formatter,
	            const sb::EdgeDetector& edgeDetector,
	            const sb::LineDetector& lineDetector );

	const cv::Mat& getColorImage() const;

	const cv::Mat& getDepthImage() const;

	const cv::Point2d& getTopLeftPoint() const;

	const std::vector<sb::LineInfo>& getLines() const;

	const std::vector<sb::LineInfo>& getWarpedLines() const;

	const std::vector<sb::SectionInfo>& getSections() const;
};
}

#endif //!__SB_FRAME_INFO_H__
