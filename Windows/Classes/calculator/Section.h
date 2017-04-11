#ifndef __SB_SECTION_H__
#define __SB_SECTION_H__

#include "../Include.h"
#include "LineInfo.h"

namespace sb
{
struct Section
{
	cv::Mat binaryImage;

	cv::Mat edgesImage;

	cv::Rect2d imageRect;

	sb::Line topLine;

	sb::Line bottomLine;

	std::vector<sb::LineInfo*> imageLines;
};

void create( sb::Section* section, 
						 const cv::Mat& containerBinaryImage, 
						 const cv::Mat& containerEdgesImage,
						 const cv::Rect& rect );

void clear( sb::Section* section );

cv::Point2d convertToContainerSpace( sb::Section* section, const cv::Point2d& pos );


}

#endif //!__SB_SECTION_H__
