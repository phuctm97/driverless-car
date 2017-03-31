#include "EdgeDetector.h"

void sb::EdgeDetector::apply( cv::Mat& image ) const
{
	cv::Canny( image,
	           image,
	           _edgeDetectorLowThresh,
	           _edgeDetectorHighThresh,
	           _edgeDetectorKernelSize );

	cv::threshold( image,
	               image,
	               _edgeDetectorBinarizeThresh,
	               _edgeDetectorBinarizeMaxValue,
	               cv::THRESH_BINARY );
}

