#include "Calculator.h"

int Calculator::init( const Params& params )
{
	// calculate crop box
	cv::Size cropSize(
	                  static_cast<int>(params.CROP_SIZE_WIDTH * params.COLOR_FRAME_SIZE.width),
	                  static_cast<int>(params.CROP_SIZE_HEIGHT * params.COLOR_FRAME_SIZE.height)
	                 );
	cv::Point cropPosition;
	cropPosition.x = (params.COLOR_FRAME_SIZE.width - cropSize.width) / 2;
	cropPosition.y = params.COLOR_FRAME_SIZE.height - cropSize.height;

	_formatter = Formatter( cv::Rect( cropPosition.x, cropPosition.y,
	                                      cropSize.width, cropSize.height ),
	                            params.WARP_SRC_QUAD,
	                            params.WARP_DST_QUAD );

	_edgeDetector = EdgeDetector( params.EDGE_DETECTOR_KERNEL_SIZE,
	                                  params.EDGE_DETECTOR_LOW_THRESH,
	                                  params.EDGE_DETECTOR_HIGH_THRESH,
	                                  params.BINARIZE_THRESH,
	                                  params.BINARIZE_MAX_VALUE );

	_lineDetector = LineDetector( params.HOUGH_LINES_P_RHO,
	                                  params.HOUGH_LINES_P_THETA,
	                                  params.HOUGH_LINES_P_THRESHOLD,
	                                  params.HOUGH_LINES_P_MIN_LINE_LENGTH,
	                                  params.HOUGH_LINES_P_MAX_LINE_GAP );

	return 0;
}

int Calculator::calculate( const RawContent& rawContent,
                               FrameInfo& frameInfo ) const
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

void Calculator::release() {}
