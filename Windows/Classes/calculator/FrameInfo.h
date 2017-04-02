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
	cv::Mat _colorImage;

	std::vector<sb::LineInfo> _imageLineInfos;

	std::vector<sb::LineInfo> _realLineInfos;

public:
	FrameInfo() {}

	void create( const sb::Params& params );

	const cv::Mat& getColorImage() const;

	void setColorImage( const cv::Mat& colorImage );

	const std::vector<sb::LineInfo>& getImageLineInfos() const;

	void setImageLineInfos( const std::vector<sb::LineInfo>& lineInfos );

	const std::vector<sb::LineInfo>& getRealLineInfos() const;

	void setRealLineInfos( const std::vector<sb::LineInfo>& warpedLines );

};
}

#endif //!__SB_FRAME_INFO_H__

