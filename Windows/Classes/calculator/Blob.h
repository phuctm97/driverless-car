#ifndef __SB_BLOB_H__
#define __SB_BLOB_H__

#include "../Include.h"

namespace sb
{
struct Blob
{
	cv::Rect box = cv::Rect( 0, 0, 0, 0 );

	cv::Point origin = cv::Point( 0, 0 );

	size_t size = 0;

	std::vector<sb::Blob*> childBlobs;
};

void release( sb::Blob* blob );
}

#endif //!__SB_BLOB_H__
