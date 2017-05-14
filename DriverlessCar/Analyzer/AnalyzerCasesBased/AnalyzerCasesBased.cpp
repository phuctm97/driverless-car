#include "AnalyzerCasesBased.h"
#include "Cases/BothLaneSolidCase.h"

int sb::AnalyzerCasesBased::init()
{
	_caseRepository = new CaseRepository( 10 );

	_obstacleFinder = new ObstacleFinder();

	return 0;
}

int sb::AnalyzerCasesBased::analyze( CollectData* collectData, CalculateData* calculateData, AnalyzeData* analyzeData )
{
	ICase* lastCase = nullptr;
	if ( _caseRepository->empty() ) {
		lastCase = new BothLaneSolidCase( _params, _obstacleFinder );
		analyzeData->state = -1;
	}
	else {
		lastCase = _caseRepository->last();
	}

	int res = lastCase->analyze( _caseRepository, collectData, calculateData, analyzeData );

	if ( !_caseRepository->empty() ) {
		analyzeData->state = _caseRepository->last()->getType();
	}
	else {
		analyzeData->state = -2;
	}
	return res;
}

void sb::AnalyzerCasesBased::release()
{
	if ( _caseRepository != nullptr ) {
		_caseRepository->release();
		delete _caseRepository;
		_caseRepository = nullptr;
	}

	if(_obstacleFinder!=nullptr ) {
		_obstacleFinder = new ObstacleFinder();
		delete _obstacleFinder;
		_obstacleFinder = nullptr;
	}

	if ( _params != nullptr ) {
		delete _params;
	}
}
