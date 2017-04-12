#ifndef __SB_SECTION_H__
#define __SB_SECTION_H__

#include "../Include.h"
#include "LineInfo.h"
#include "Blob.h"

namespace sb
{
struct Section
{
	cv::Mat bgrImage;

	cv::Mat binImage;

	cv::Mat edgImage;

	cv::Rect imageRect;

	sb::Line topLine;

	sb::Line bottomLine;

	std::vector<sb::Blob*> blobs;
};

void create( sb::Section* section,
						 const cv::Mat& containerBgrImage,
						 const cv::Mat& containerBinImage,
						 const cv::Mat& containerEdgImage,
             const cv::Rect& rect );

void release( sb::Section* section );

cv::Point convertToContainerSpace( sb::Section* section, const cv::Point& pos );

cv::Point2f convertToContainerSpace( sb::Section* section, const cv::Point2f& pos );
}

#endif //!__SB_SECTION_H__
