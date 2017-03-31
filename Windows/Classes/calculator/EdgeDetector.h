#ifndef __SB_EDGE_DETECTOR_H__
#define __SB_EDGE_DETECTOR_H__

#include <opencv2/opencv.hpp>

namespace sb
{
class EdgeDetector
{
private:
	int _edgeDetectorKernelSize;
	double _edgeDetectorLowThresh;
	double _edgeDetectorHighThresh;
	double _edgeDetectorBinarizeThresh;
	double _edgeDetectorBinarizeMaxValue;

public:
	EdgeDetector() {}

	EdgeDetector( int edgeDetectorKernelSize,
	              double edgeDetectorLowThresh,
	              double edgeDetectorHighThresh,
	              double edgeDetectorBinarizeThresh,
	              double edgeDetectorBinarizeMaxValue )
		: _edgeDetectorKernelSize( edgeDetectorKernelSize ),
		  _edgeDetectorLowThresh( edgeDetectorLowThresh ),
		  _edgeDetectorHighThresh( edgeDetectorHighThresh ),
		  _edgeDetectorBinarizeThresh( edgeDetectorBinarizeThresh ),
		  _edgeDetectorBinarizeMaxValue( edgeDetectorBinarizeMaxValue ) {}

	void apply( cv::Mat& image ) const;
};
}

#endif //!__SB_EDGE_DETECTOR_H__

