#ifndef __I_ANALYZER_H__
#define __I_ANALYZER_H__
#include "../Collector/Data/CollectData.h"
#include "../Calculator/Data/CalculateData.h"
#include "Data/AnalyzeData.h"

namespace sb
{
class IAnalyzer
{
public:
	virtual ~IAnalyzer() = default;

	virtual int init() = 0;

	virtual int analyze( CollectData* collectData, CalculateData* calculateData, AnalyzeData* analyzeData ) = 0;

	virtual void release() = 0;
};
}

#endif //!__I_ANALYZER_H__
