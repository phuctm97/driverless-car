#ifndef __SB_COLLECT_DATA_H__
#define __SB_COLLECT_DATA_H__

#include <opencv2/opencv.hpp>

namespace sb
{
struct CollectData
{
	cv::Mat colorImage;
};

void release( CollectData* collectData );

}

#endif //!__SB_COLLECT_DATA_H__