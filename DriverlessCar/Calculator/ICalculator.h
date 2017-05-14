#ifndef __I_CALCULATOR_H__
#define __I_CALCULATOR_H__

#include "../Collector/Data/CollectData.h"
#include "Data/CalculateData.h"

namespace sb
{
class ICalculator
{
public:
	virtual ~ICalculator() = default;

	virtual int init() = 0;

	virtual int calculate( CollectData* collectData, CalculateData* calculateData ) = 0;

	virtual void release() = 0;
};
}

#endif //!__I_CALCULATOR_H__
