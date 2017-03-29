#ifndef __SB_SECTION_INFO_H__
#define __SB_SECTION_INFO_H__

#include "../Include.h"

namespace sb
{
struct SectionInfo
{
	std::vector<std::pair<int, cv::Vec2d>> lines;

	int upperRow;

	int lowerRow;

	SectionInfo()
		: upperRow( 0 ), lowerRow( 0 ) {}
};

}

#endif //!__SB_SECTION_INFO_H__