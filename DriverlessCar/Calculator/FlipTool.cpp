#include "FlipTool.h"

int sb::FlipTool::flip( const cv::Mat& inputImage, cv::Mat& outputImage )
{
	cv::flip( inputImage, outputImage, 1 );

	return 0;
}
