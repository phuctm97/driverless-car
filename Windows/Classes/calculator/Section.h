#ifndef __SB_SECTION_H__
#define __SB_SECTION_H__

#include "../Include.h"
#include "LineInfo.h"
#include "Blob.h"

namespace sb
{
struct Section
{
	cv::Mat binaryImage;

	cv::Rect imageRect;

	sb::Line topLine;

	sb::Line bottomLine;

	std::vector<sb::Blob*> blobs;
};

void create( sb::Section* section,
             const cv::Mat& containerBinaryImage,
             const cv::Rect& rect );

void clear( sb::Section* section );

cv::Point convertToContainerSpace( sb::Section* section, const cv::Point& pos );

cv::Point2f convertToContainerSpace( sb::Section* section, const cv::Point2f& pos );
}

#endif //!__SB_SECTION_H__
