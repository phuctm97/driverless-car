#ifndef __SB_FRAME_INFO_H__
#define __SB_FRAME_INFO_H__

#include "../Params.h"
#include "Section.h"

namespace sb
{
struct FrameInfo
{
	cv::Mat colorImage;

	cv::Mat binaryImage;

	std::vector<sb::Section*> imageSections;
};

void create( sb::FrameInfo* frameInfo, sb::Params* params );

void clear( sb::FrameInfo* frameInfo );

}

#endif //!__SB_FRAME_INFO_H__
