#ifndef __SB_COLLECTOR_H__
#define __SB_COLLECTOR_H__

#include "../Params.h"
#include "RawContent.h"

namespace sb
{
class Collector
{
private:
	// sample input stream
	cv::VideoCapture _tempCap;

public:
	Collector() {}

	int init( const sb::Params& params );

	int collect( sb::RawContent& rawContent );

	void release();
};
}

#endif //!__SB_COLLECTOR_H__
