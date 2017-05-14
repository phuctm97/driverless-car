#ifndef __SB_APPLICATION_TEST_COLLECTOR_H__
#define __SB_APPLICATION_TEST_COLLECTOR_H__
#include "../IApplication.h"
#include "../../Collector/ICollector.h"

namespace sb
{
class ApplicationTestCollector : public IApplication
{
private:
	IKeyboard* _keyboard;

	ICollector* _collector;

	bool _exiting;

	cv::String _videoPath;

public:
	ApplicationTestCollector( ICollector* collector, IKeyboard* keyboard, const cv::String& videoPath = "" )
		: _exiting( false ), _collector( collector ), _keyboard( keyboard ), _videoPath( videoPath ) {}

	void run() override;

	void exit() override;

	void release() override;

	void addKeyboardCallback( const std::function<void( int )>& callback ) override;
};
}

#endif //!__SB_APPLICATION_TEST_COLLECTOR_H__
