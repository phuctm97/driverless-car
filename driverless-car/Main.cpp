#include "../Classes/calculator/Calculator.h"
#include "../Classes/collector/Collector.h"
#include "../Classes/analyzer/Analyzer.h"
#include "../Classes/driver/Driver.h"
#include "../Classes/Timer.h"

int init( sb::Collector& collector,
          sb::Calculator& calculator,
          sb::Analyzer& analyzer,
          sb::Driver& driver,
          const sb::Params& params );

void release( sb::Collector& collector,
              sb::Calculator& calculator,
              sb::Analyzer& analyzer,
              sb::Driver& driver );

int main()
{
	// Parameters container for every component
	sb::Params params;
	params.load( PARAMS_PATH );

	// Timer for performance test
	sb::Timer timer;

	// Data sent&receive bewteen components
	sb::RawContent rawContent;
	sb::FrameInfo frameInfo;
	sb::RoadInfo roadInfo;

	// Main Components
	sb::Collector collector;
	sb::Calculator calculator;
	sb::Analyzer analyzer;
	sb::Driver driver;

	// Init components
	if ( init( collector, calculator, analyzer, driver, params ) < 0 ) {
		std::cerr << "Init failed." << std::endl;
		return -1;
	}

	// Initial values
	rawContent.create( params );
	frameInfo.create( params );
	roadInfo.create( params );

	while ( true ) {
		
		/* For performance test
		timer.reset( "total" );
		*/

		if ( collector.collect( rawContent ) < 0 ) {
			std::cerr << "Collector collects failed." << std::endl;
			break;
		}

		if ( calculator.calculate( rawContent, frameInfo ) < 0 ) {
			std::cerr << "Calculator calculates failed." << std::endl;
			break;
		}

		if ( analyzer.analyze( frameInfo, roadInfo ) ) {
			std::cerr << "Analyzer analyzes failed." << std::endl;
			break;
		}

		if( driver.drive( roadInfo) < 0 ) {
			std::cerr << "Driver drives failed." << std::endl;
			break;
		}

		/* For performance test
		std::cout
				<< "Executed time: " << timer.milliseconds( "total" ) << ". "
				<< "FPS: " << timer.fps( "total" ) << std::endl;
		*/

		if ( cv::waitKey( /*MAX( 1, 66 - timer.milliseconds( "total" ) )*/ ) /*== KEY_TO_ESCAPE*/ ) break;
	}

	release( collector, calculator, analyzer, driver );
	return 0;
}

int init( sb::Collector& collector,
          sb::Calculator& calculator,
          sb::Analyzer& analyzer,
          sb::Driver& driver,
          const sb::Params& params )
{
	if ( collector.init( params ) < 0 ) {
		std::cerr << "Collector init failed." << std::endl;
		return -1;
	}

	if ( calculator.init( params ) < 0 ) {
		std::cerr << "Calculator init failed." << std::endl;
		return -1;
	}

	if ( analyzer.init( params ) < 0 ) {
		std::cerr << "Analyzer init failed." << std::endl;
		return -1;
	}

	if ( driver.init( params ) < 0 ) {
		std::cerr << "Driver init failed." << std::endl;
		return -1;
	}

	return 0;
}

void release( sb::Collector& collector,
              sb::Calculator& calculator,
              sb::Analyzer& analyzer,
              sb::Driver& driver )
{
	calculator.release();

	collector.release();

	analyzer.release();

	driver.release();
}
