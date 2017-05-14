#ifndef __SB_COLLECTOR_WITH_VIDEO_H__
#define __SB_COLLECTOR_WITH_VIDEO_H__

#include <opencv2/opencv.hpp>
#include "../ICollector.h"

namespace sb
{
class CollectorWithVideo: public ICollector
{
private:
	cv::VideoCapture _stream;

public:
	CollectorWithVideo( const std::string& fileName );

	int init() override;

	int collect( CollectData* collectData ) override;

	void release() override;
};
}

#endif //!__SB_COLLECTOR_WITH_VIDEO_H__
