#ifndef __SB_I_CASE_H__
#define __SB_I_CASE_H__

#include "../../../Collector/Data/CollectData.h"
#include "../../../Calculator/Data/CalculateData.h"
#include "../../Data/AnalyzeData.h"

namespace sb
{
class CaseRepository;

enum CaseType
{
	BOTH_LANE_SOLID_CASE = 0,
	LEFT_LANE_SOLID_CASE = 1,
	RIGHT_LANE_SOLID_CASE = 2,
	OBSTACLE_ON_LEFT_LANE_CASE = 3,
	OBSTACLE_ON_RIGHT_LANE_CASE = 4
};

class ICase
{
public:
	virtual ~ICase() = default;

	virtual int getType() = 0;

	virtual int analyze( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, AnalyzeData* analyzeData ) = 0;
};
}

#endif //!__SB_I_CASE_H__
