#ifndef __SB_I_COLLECTOR_H__
#define __SB_I_COLLECTOR_H__

#include <opencv2/opencv.hpp>
#include "Data/CollectData.h"

namespace sb
{
class ICollector
{
public:
	virtual ~ICollector() = default;

	virtual int init() = 0;

	virtual int collect( CollectData* collectData ) = 0;

	virtual void release() = 0;
};
}

#endif //!__SB_I_COLLECTOR_H__
