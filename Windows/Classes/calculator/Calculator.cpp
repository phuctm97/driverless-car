#include "Calculator.h"

int sb::Calculator::init( const sb::Params& params )
{
	_formatter = sb::Formatter( cv::Size2d( params.CROP_SIZE_WIDTH, params.CROP_SIZE_HEIGHT ) );

	_edgeDetector = sb::EdgeDetector( params.EDGE_DETECTOR_KERNEL_SIZE,
	                                  params.EDGE_DETECTOR_LOW_THRESH,
	                                  params.EDGE_DETECTOR_HIGH_THRESH,
	                                  params.BINARIZE_THRESH,
	                                  params.BINARIZE_MAX_VALUE );

	_lineDetector = sb::LineDetector( params.HOUGH_LINES_P_RHO,
	                                  params.HOUGH_LINES_P_THETA,
	                                  params.HOUGH_LINES_P_THRESHOLD,
	                                  params.HOUGH_LINES_P_MIN_LINE_LENGTH,
	                                  params.HOUGH_LINES_P_MAX_LINE_GAP );

	return 0;
}

int sb::Calculator::calculate( const sb::RawContent& rawContent,
                               sb::FrameInfo& frameInfo ) const
{
	if ( frameInfo.create( rawContent.getColorImage(),
	                       rawContent.getDepthImage(),
	                       _formatter,
	                       _edgeDetector,
	                       _lineDetector ) < 0 ) {
		std::cerr << "Calculate frame info failed." << std::endl;
		return -1;
	}

	return 0;
}

void sb::Calculator::release() {}
