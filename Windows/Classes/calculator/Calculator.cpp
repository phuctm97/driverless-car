#include "Calculator.h"

int sb::Calculator::init( const sb::Params& params )
{
	// calculate crop box
	cv::Size cropSize(
	                  static_cast<int>(params.CROP_SIZE_WIDTH * params.COLOR_FRAME_SIZE.width),
	                  static_cast<int>(params.CROP_SIZE_HEIGHT * params.COLOR_FRAME_SIZE.height)
	                 );
	cv::Point cropPosition;
	cropPosition.x = (params.COLOR_FRAME_SIZE.width - cropSize.width) / 2;
	cropPosition.y = params.COLOR_FRAME_SIZE.height - cropSize.height;

	_formatter = sb::Formatter( cv::Rect( cropPosition.x, cropPosition.y,
	                                      cropSize.width, cropSize.height ),
	                            params.SEPERATE_ROWS,
	                            params.WARP_SRC_QUAD,
	                            params.WARP_DST_QUAD );

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
