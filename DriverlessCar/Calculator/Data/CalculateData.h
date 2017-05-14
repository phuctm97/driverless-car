#ifndef __SB_CALCULATE_DATA_H__
#define __SB_CALCULATE_DATA_H__
#include "Blob.h"

namespace sb
{
struct CalculateData
{
	cv::Mat bgrImage;

	cv::Mat binImage;

	std::vector<Blob*> blobs;
};

void release( CalculateData* calculateData, bool releaseBlobs = false );
}

#endif //!__SB_CALCULATE_DATA_H__
