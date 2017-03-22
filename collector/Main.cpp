#include "../Classes/collector/Collector.h"

int main()
{
	sb::RawContent rawContent;

	sb::Collector collector;

	if ( collector.init() < 0 ) {
		std::cerr << "Collector init failed." << std::endl;
		return -1;
	}

	while ( true ) {
	
		if ( collector.collect( rawContent ) < 0 ) {
			std::cerr << "Collector collect failed." << std::endl;
			collector.release();
			return -1;
		}

		// sample test
		cv::imshow( WINDOW_NAME, rawContent.getColorImage() );
		if( cv::waitKey( 33 ) == KEY_TO_ESCAPE ) break;
	}

	collector.release();
	return 0;
}
