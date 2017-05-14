#ifndef __SB_APPLICATION_TEST_CALCULATOR_H__
#define __SB_APPLICATION_TEST_CALCULATOR_H__
#include "../IApplication.h"
#include "../../Collector/ICollector.h"
#include "../../Calculator/ICalculator.h"

namespace sb
{
class ApplicationTestCalculator : public IApplication
{
private:
	IKeyboard* _keyboard;

	ICollector* _collector;

	ICalculator* _calculator;

	bool _exiting;

public:
	ApplicationTestCalculator( ICollector* collector, ICalculator* calculator, IKeyboard* keyboard )
		: _exiting( false ), _collector( collector ), _calculator( calculator ), _keyboard( keyboard ) {}

	void run() override;

	void exit() override;

	void release() override;

	void addKeyboardCallback( const std::function<void( int )>& callback ) override;

private:
	void showResult( sb::CalculateData* calculateData );

};
}

#endif //!__SB_APPLICATION_TEST_CALCULATOR_H__
