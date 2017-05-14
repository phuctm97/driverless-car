#ifndef __SB_DRIVER_STUPID_H__
#define __SB_DRIVER_STUPID_H__
#include "../IDriver.h"

namespace sb
{
class DriverStupid : public IDriver
{
public:
	int init() override;

	int drive( AnalyzeData* analyzeData ) override;

	void release() override;
};
}

#endif //!__SB_DRIVER_STUPID_H__