#ifndef __SB_FORMATTER_H__
#define __SB_FORMATTER_H__

#include <opencv2/opencv.hpp>

namespace sb
{
class Formatter
{
private:
	cv::Size2d _cropSize;

public:
	Formatter() {}

	Formatter( const cv::Size2d& cropSize )
		: _cropSize( cropSize ) {}

	int crop( const cv::Mat& inputImage, cv::Mat& outputImage ) const;
};
}

#endif //!__SB_FORMATTER_H__
