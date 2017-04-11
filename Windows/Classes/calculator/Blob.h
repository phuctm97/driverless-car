#ifndef __SB_BLOB_H__
#define __SB_BLOB_H__

#include "../Include.h"


namespace sb
{
struct Blob
{
	std::vector<cv::Point> pixels;

	cv::Vec3b bgr;

	int minX, maxX, minY, maxY;
};

}

#endif //!__SB_BLOB_H__