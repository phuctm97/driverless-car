#ifndef __SB_FRAME_INFO_H__
#define __SB_FRAME_INFO_H__

#include "../Params.h"
#include "Section.h"

namespace sb
{
struct FrameInfo
{
	cv::Mat colorImage;

	cv::Mat edgesImage;

	std::vector<sb::Section> imageSections;
};

void construct( sb::FrameInfo*& frameInfo );

void destruct( sb::FrameInfo*& frameInfo );

void create( sb::FrameInfo* frameInfo, sb::Params* params );

}

#endif //!__SB_FRAME_INFO_H__
