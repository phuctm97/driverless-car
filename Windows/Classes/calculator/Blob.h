#ifndef __SB_BLOB_H__
#define __SB_BLOB_H__

#include "../Include.h"


namespace sb
{
struct Blob
{
	std::vector<cv::Point> pixels;

	cv::Vec3b bgr;

	cv::Rect box;

	cv::Point origin;
};

void release( sb::Blob* blob );

}

#endif //!__SB_BLOB_H__