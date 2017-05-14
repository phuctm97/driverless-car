#include "Collector/ICollector.h"
#include "Collector/CollectorWithVideo/CollectorWithVideo.h" // [on Car] replace with CollectorWithCamera
#include "Calculator/ICalculator.h"
#include "Calculator/CalculatorBlobsBased/CalculatorBlobsBased.h"
#include "Calculator/CropTool.h"
#include "Calculator/FlipTool.h"
#include "Calculator/BinarizeTool.h"
#include "Calculator/BlobTool.h"
#include "Analyzer/IAnalyzer.h"
#include "Analyzer/AnalyzerCasesBased/AnalyzerCasesBased.h"
#include "Driver/IDriver.h"
#include "Driver/DriverStupid/DriverStupid.h"	// [on Car] replace with DriverPid
#include "Application/Keyboard/WindowsKeyboard.h" // [on Car] replace with LinuxKeyboard
#include "Application/IApplication.h"
#include "Application/ApplicationTestCollector/ApplicationTestCollector.h"
#include "Application/ApplicationTestCalculator/ApplicationTestCalculator.h"
#include "Application/ApplicationTestAnalyzer/ApplicationTestAnalyzer.h"
#include "Application/ApplicationFinal/ApplicationFinal.h"

sb::IApplication* application = nullptr;

void composeApplicationTestCollector();

void composeApplicationTestCalculator();

void composeApplicationTestAnalyzer();

void composeApplicationFinal();

void releaseApplication();

void onKeyPressed( int key );

int main( const int argc, const char** argv )
{
	composeApplicationTestAnalyzer();

	if ( application == nullptr ) return -1;

	application->addKeyboardCallback( &onKeyPressed );

	application->run();

	releaseApplication();

	system( "pause" );

	return 0;
}

void composeApplicationTestCollector()
{
	sb::ICollector* collector = nullptr;

	collector = new sb::CollectorWithVideo( "..\\Debug\\sample-5.avi" ); // [on Car] replace with CollectorWithCamera

	application = new sb::ApplicationTestCollector( collector, new sb::WindowsKeyboard( 33 ) ); // [on Car] replace with LinuxKeyboard
}

void composeApplicationTestCalculator()
{
	sb::ICollector* collector = nullptr;

	sb::ICalculator* calculator = nullptr;

	collector = new sb::CollectorWithVideo( "..\\Debug\\sample-9.avi" ); // [on Car] replace with CollectorWithCamera

	calculator = new sb::CalculatorBlobsBased( new sb::CropTool( cv::Rect( 0, 332, 640, 100 ) ),
	                                           new sb::FlipTool(),
	                                           new sb::BinarizeTool( 170 ),
	                                           new sb::BlobTool( { 0.2,0.25,0.25,0.3 }, cv::Size( 640, 100 ) ) );

	application = new sb::ApplicationTestCalculator( collector, calculator, new sb::WindowsKeyboard( 33 ) ); // [on Car] replace with LinuxKeyboard
}

void composeApplicationTestAnalyzer()
{
	sb::ICollector* collector = nullptr;

	sb::ICalculator* calculator = nullptr;

	sb::IAnalyzer* analyzer = nullptr;

	collector = new sb::CollectorWithVideo( "..\\Debug\\sample-13.avi" ); // [on Car] replace with CollectorWithCamera

	calculator = new sb::CalculatorBlobsBased( new sb::CropTool( cv::Rect( 0, 332, 640, 100 ) ),
	                                           new sb::FlipTool(),
	                                           new sb::BinarizeTool( 190 ),
	                                           new sb::BlobTool( { 0.2,0.25,0.25,0.3 }, cv::Size( 640, 100 ) ) );

	sb::AnalyzeParams* analyzeParams = new sb::AnalyzeParams(); {
		analyzeParams->MIN_LANE_BLOB_SIZE = 1200;
		analyzeParams->MIN_LANE_BLOB_HEIGHT = 50;
		analyzeParams->MIN_LANE_BLOB_HEIGHT_TO_CHECK_OBSTACLE = 70;
		analyzeParams->MIN_LANE_WIDTH_1 = 30;
		analyzeParams->MAX_LANE_WIDTH_1 = 70;
		analyzeParams->MIN_LANE_WIDTH_2 = 20;
		analyzeParams->MAX_LANE_WIDTH_2 = 50;
		analyzeParams->MAX_ROW_WIDTH_DIFF = 7;
		analyzeParams->SECTION_HOPS_TO_LIVE = 4;
		analyzeParams->MAX_LANE_POSITION_DIFF = 100;
		analyzeParams->MAX_LANE_SIZE_DIFF = 2500;
		analyzeParams->MAX_LANE_HEIGHT_DIFF = 30;
		analyzeParams->CROP_OFFSET = cv::Point( 0, 332 );
	}
	analyzer = new sb::AnalyzerCasesBased( analyzeParams );

	application = new sb::ApplicationTestAnalyzer( collector, calculator, analyzer, new sb::WindowsKeyboard( 33 ) ); // [on Car] replace with LinuxKeyboard
}

void composeApplicationFinal()
{
	sb::ICollector* collector = nullptr;

	sb::ICalculator* calculator = nullptr;

	sb::IAnalyzer* analyzer = nullptr;

	sb::IDriver* driver = nullptr;

	collector = new sb::CollectorWithVideo( "..\\Debug\\sample-1.avi" ); // [on Car] replace with CameraWithCamera

	calculator = new sb::CalculatorBlobsBased( new sb::CropTool( cv::Rect( 0, 332, 640, 100 ) ),
	                                           new sb::FlipTool(),
	                                           new sb::BinarizeTool( 200 ),
	                                           new sb::BlobTool( { 0.2,0.25,0.25,0.3 }, cv::Size( 640, 100 ) ) );

	sb::AnalyzeParams* analyzeParams = new sb::AnalyzeParams(); {
		analyzeParams->MIN_LANE_BLOB_SIZE = 1500;
		analyzeParams->MIN_LANE_BLOB_HEIGHT = 75;
		analyzeParams->MIN_LANE_WIDTH_1 = 30;
		analyzeParams->MAX_LANE_WIDTH_1 = 70;
		analyzeParams->MIN_LANE_WIDTH_2 = 50;
		analyzeParams->MAX_LANE_WIDTH_2 = 20;
		analyzeParams->MAX_ROW_WIDTH_DIFF = 7;
		analyzeParams->SECTION_HOPS_TO_LIVE = 4;
		analyzeParams->MAX_LANE_POSITION_DIFF = 100;
		analyzeParams->MAX_LANE_SIZE_DIFF = 1000;
		analyzeParams->MAX_LANE_HEIGHT_DIFF = 30;
	}
	analyzer = new sb::AnalyzerCasesBased( analyzeParams );

	driver = new sb::DriverStupid(); // [on Car] replace with DriverPid

	application = new sb::ApplicationFinal( collector, calculator, analyzer, driver, new sb::WindowsKeyboard( 33 ), "D:\\test", cv::Point( 0, 332 ) ); // [on Car] replace with LinuxKeyboard
}

void releaseApplication()
{
	application->release();
}

void onKeyPressed( int key )
{
	if ( key == 27 ) application->exit();
}
