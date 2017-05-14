#ifndef __SB_APPLICATION_TEST_ANALYZER_H__
#define __SB_APPLICATION_TEST_ANALYZER_H__
#include "../IApplication.h"
#include "../../Collector/ICollector.h"
#include "../../Calculator/ICalculator.h"
#include "../../Analyzer/IAnalyzer.h"

namespace sb
{
class ApplicationTestAnalyzer : public IApplication
{
private:
	IKeyboard* _keyboard;

	ICollector* _collector;

	ICalculator* _calculator;

	IAnalyzer* _analyzer;

	bool _exiting;

public:
	ApplicationTestAnalyzer( ICollector* collector, ICalculator* calculator, IAnalyzer* analyzer, IKeyboard* keybaord )
		: _exiting( false ), _collector( collector ), _calculator( calculator ), _analyzer( analyzer ), _keyboard( keybaord ) {}
	
	void run() override;
	
	void exit() override;

	void release() override;

	void addKeyboardCallback( const std::function<void(int)>& callback ) override;

};
}

#endif //!__SB_APPLICATION_TEST_ANALYZER_H__
