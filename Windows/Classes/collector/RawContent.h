#ifndef __SB_RAW_CONTENT_H__
#define __SB_RAW_CONTENT_H__

#include "../Params.h"

namespace sb
{

struct RawContent
{
	cv::Mat colorImage;
};

void create( sb::RawContent* rawContent, sb::Params* params );

}

#endif //!__SB_RAW_CONTENT_H__
