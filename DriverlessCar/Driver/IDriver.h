#ifndef __SB_I_DRIVER_H__
#define __SB_I_DRIVER_H__

#include "../Analyzer/Data/AnalyzeData.h"

namespace sb
{
class IDriver
{
public:
	virtual ~IDriver() = default;

	virtual int init() = 0;

	virtual int drive( AnalyzeData* analyzeData ) = 0;

	virtual void release() = 0;
};
}

#endif //!__SB_I_DRIVER_H__
