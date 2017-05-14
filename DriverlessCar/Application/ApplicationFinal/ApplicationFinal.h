#ifndef __SB_APPLICATION_FINAL_H__
#define __SB_APPLICATION_FINAL_H__
#include "../IApplication.h"
#include "../../Collector/ICollector.h"
#include "../../Calculator/ICalculator.h"
#include "../../Analyzer/IAnalyzer.h"
#include "../../Driver/IDriver.h"

namespace sb
{
class ApplicationFinal : public IApplication
{
private:
	IKeyboard* _keyboard;

	ICollector* _collector;

	ICalculator* _calculator;

	IAnalyzer* _analyzer;

	IDriver* _driver;

	bool _exiting;

	cv::String _videoPath;

	cv::Point _cropOffset;

public:
	ApplicationFinal( ICollector* collector, ICalculator* calculator, IAnalyzer* analyzer, IDriver* driver, IKeyboard* keyboard, const cv::String& videoPath = "", const cv::Point cropOffset = cv::Point( 0, 0 ) )
		: _exiting( false ), _collector( collector ), _calculator( calculator ), _analyzer( analyzer ), _driver( driver ), _keyboard( keyboard ), _videoPath( videoPath ), _cropOffset( cropOffset ) {}

	void run() override;

	void exit() override;

	void release() override;

	void addKeyboardCallback( const std::function<void( int )>& callback ) override;
};
}

#endif //!__SB_APPLICATION_FINAL_H__
