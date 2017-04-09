#ifndef __SB_SECTION_H__
#define __SB_SECTION_H__

#include "../Include.h"
#include "LineInfo.h"

namespace sb
{
class Section
{
	cv::Mat _binaryImage;

	// rect in container-image
	cv::Rect2d _imageRect;

	// detected line with LineDetector
	std::vector<sb::LineInfo> _imageLines;

public:
	Section( const cv::Mat& containerBinaryImage, const cv::Rect& rect );

	const cv::Mat& getBinaryImage() const;

	void setBinaryImage( const cv::Mat& binaryImage );

	const cv::Rect2d& getImageRect() const;

	void setImageRect( const cv::Rect2d& imageRect );

	const std::vector<sb::LineInfo>& getImageLines() const;

	void setImageLines( const std::vector<sb::LineInfo>& imageLines );

	sb::Line getBottomLine() const;
	
	sb::Line getTopLine() const;

	cv::Point2d convertToContainerSpace( const cv::Point2d& pos ) const;

};


}

#endif //!__SB_SECTION_H__