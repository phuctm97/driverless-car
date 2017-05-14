#ifndef __SB_COLLECTOR_H__
#define __SB_COLLECTOR_H__

#include "RawContent.h"

namespace sb
{
struct Collector
{
	// sample input stream
	cv::VideoCapture tempCap;
};

int init( sb::Collector* collector, sb::Params* params );

int collect( sb::Collector* collector, sb::RawContent* rawContent );

void release( sb::Collector* collector );
}

#endif //!__SB_COLLECTOR_H__
