#ifndef __SB_FRAME_INFO_H__
#define __SB_FRAME_INFO_H__

#include "LineInfo.h"
#include "Formatter.h"
#include "EdgeDetector.h"
#include "LineDetector.h"

namespace sb
{
class FrameInfo
{
private:
	cv::Mat _colorImage;

	cv::Mat _depthImage;

	std::vector<sb::LineInfo> _lines;
public:
	FrameInfo();

	int create( const cv::Mat& colorImage,
							 const cv::Mat& depthImage,
	             const sb::Formatter& formatter,
	             const sb::EdgeDetector& edgeDetector,
	             const sb::LineDetector& lineDetector );

	const cv::Mat& getColorImage() const;

	const cv::Mat& getDepthImage() const;

	const std::vector<sb::LineInfo>& getLines() const;
};
}

#endif //!__SB_FRAME_INFO_H__
