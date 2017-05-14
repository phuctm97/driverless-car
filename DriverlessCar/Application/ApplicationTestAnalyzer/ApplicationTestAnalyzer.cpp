#include "ApplicationTestAnalyzer.h"
#include <ctime>
#include "../../Analyzer/AnalyzerCasesBased/Cases/ICase.h"

void sb::ApplicationTestAnalyzer::run()
{
	// init
	_exiting = false;

	sb::CollectData* collectData = new CollectData;
	sb::CalculateData* calculateData = new CalculateData;
	sb::AnalyzeData* analyzeData = new AnalyzeData;

	if ( _collector == nullptr || _collector->init() < 0 ) _exiting = true;
	if ( _calculator == nullptr || _calculator->init() < 0 ) _exiting = true;
	if ( _analyzer == nullptr || _analyzer->init() < 0 ) _exiting = true;

	// debug
	cv::Mat videoFrame;
	int frameCount = 0;

	// run
	clock_t t;

	while ( !_exiting ) {

		t = clock();
		if ( _collector->collect( collectData ) < 0 ) break;
		std::cout << "Collector: " << 1000 * (clock() - t) / CLOCKS_PER_SEC << std::endl;

		// debug
		std::cout << frameCount++ << std::endl;

		t = clock();
		if ( _calculator->calculate( collectData, calculateData ) < 0 ) break;
		std::cout << "Calculator: " << 1000 * (clock() - t) / CLOCKS_PER_SEC << std::endl;

		t = clock();
		if ( _analyzer->analyze( collectData, calculateData, analyzeData ) < 0 ) break;
		std::cout << "Analyzer: " << 1000 * (clock() - t) / CLOCKS_PER_SEC << std::endl;

		// debug
		{
			videoFrame = collectData->colorImage.clone();
			cv::rectangle( videoFrame, cv::Point( 0, 332 ), cv::Point( 0 + 640, 332 + 100 ), cv::Scalar( 0, 0, 255 ), 2 );
			cv::circle( videoFrame, analyzeData->target + cv::Point( 0, 332 ), 5, cv::Scalar( 0, 255, 0 ), 3 );

			std::string state = "init";
			switch ( analyzeData->state ) {
			case -1: state = "init"; break;
			case -2: state = "stop"; break;
			case CaseType::RIGHT_LANE_SOLID_CASE: state = "right lane solid"; break;
			case CaseType::LEFT_LANE_SOLID_CASE: state = "left lane solid"; break;
			case CaseType::BOTH_LANE_SOLID_CASE: state = "both lane solid"; break;
			case CaseType::OBSTACLE_ON_LEFT_LANE_CASE: state = "obstacle left"; break;
			}
			cv::putText( videoFrame, state, cv::Point( 30, 30 ), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar( 0, 0, 255 ), 2 );

			cv::imshow( "Analyzer", videoFrame );
			cv::waitKey();
		}

		// keyboard event
		if ( _keyboard != nullptr ) {
			_keyboard->checkBufferedKey();
		}
	}

	// release
	if ( collectData != nullptr ) {
		sb::release( collectData );
		delete collectData;
	}

	if ( calculateData != nullptr ) {
		sb::release( calculateData );
		delete calculateData;
	}

	if ( analyzeData != nullptr ) {
		sb::release( analyzeData );
		delete analyzeData;
	}
}

void sb::ApplicationTestAnalyzer::exit()
{
	_exiting = true;
}

void sb::ApplicationTestAnalyzer::release()
{
	if ( _keyboard != nullptr ) {
		_keyboard->release();
		delete _keyboard;
	}

	if ( _collector != nullptr ) {
		_collector->release();
		delete _collector;
	}

	if ( _calculator != nullptr ) {
		_calculator->release();
		delete _calculator;
	}

	if ( _analyzer != nullptr ) {
		_analyzer->release();
		delete _analyzer;
	}
}

void sb::ApplicationTestAnalyzer::addKeyboardCallback( const std::function<void( int )>& callback )
{
	if ( _keyboard != nullptr ) _keyboard->addKeyboardCallback( callback );
}
