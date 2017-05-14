#ifndef __SB_FRAME_INFO_H__
#define __SB_FRAME_INFO_H__

#include "../Params.h"
#include "Blob.h"

namespace sb
{
struct FrameInfo
{
	cv::Mat bgrImage;

	cv::Mat binImage;

	cv::Mat edgImage;

	std::vector<sb::Blob*> blobs;
};

void create( sb::FrameInfo* frameInfo, sb::Params* params );

void release( sb::FrameInfo* frameInfo, bool releaseBlobs = true );
}

#endif //!__SB_FRAME_INFO_H__

