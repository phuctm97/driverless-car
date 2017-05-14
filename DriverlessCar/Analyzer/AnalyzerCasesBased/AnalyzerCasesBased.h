#ifndef __SB_ANALYZER_CASES_BASED_H__
#define __SB_ANALYZER_CASES_BASED_H__

#include "../IAnalyzer.h"
#include "CaseRepository.h"
#include "../Data/Params/AnalyzeParams.h"
#include "ObstacleFinder.h"

namespace sb
{
class AnalyzerCasesBased : public IAnalyzer
{
private:
	CaseRepository* _caseRepository;

	ObstacleFinder* _obstacleFinder;

	AnalyzeParams* _params;

public:
	AnalyzerCasesBased( AnalyzeParams* params )
		: _caseRepository( nullptr ), _obstacleFinder( nullptr ), _params( params ) { }

	int init() override;

	int analyze( CollectData* collectData, CalculateData* calculateData, AnalyzeData* analyzeData ) override;

	void release() override;
};
}

#endif //!__SB_ANALYZER_CASES_BASED_H__
