#include "../Classes/collector/Collector.h"

int init( sb::Collector& collector,
          const sb::Params& params );

void test( const sb::RawContent& rawContent );

void release( sb::Collector& collector );

int main()
{
	sb::Params params;
	params.load( PARAMS_PATH );

	sb::RawContent rawContent;

	sb::Collector collector;

	if ( init( collector, params ) < 0 ) {
		std::cerr << "Init failed." << std::endl;
		return -1;
	}

	while ( true ) {

		if ( collector.collect( rawContent ) < 0 ) {
			std::cerr << "Collector collect failed." << std::endl;
			release( collector );
			return -1;
		}

		test( rawContent );

		if ( cv::waitKey( 33 ) == KEY_TO_ESCAPE ) break;
	}

	release( collector );
	return 0;
}

int init( sb::Collector& collector,
          const sb::Params& params )
{
	if ( collector.init( params ) < 0 ) {
		std::cerr << "Collector init failed." << std::endl;
		return -1;
	}

	return 0;
}

void test( const sb::RawContent& rawContent )
{
	cv::imshow( WINDOW_NAME, rawContent.getColorImage() );
}

void release( sb::Collector& collector )
{
	collector.release();
}
