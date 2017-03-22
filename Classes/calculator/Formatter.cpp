#include "Formatter.h"

int sb::Formatter::crop( const cv::Mat& inputImage, cv::Mat& outputImage ) const
{
	if ( _cropSize.width > 1 || _cropSize.height > 1 ) {
		std::cerr << "Invalid crop size." << std::endl;
		return -1;
	}

	cv::Size cropRealSize(
	                      static_cast<int>(_cropSize.width * inputImage.cols),
	                      static_cast<int>(_cropSize.height * inputImage.rows)
	                     );

	int x, y;
	x = (inputImage.cols - cropRealSize.width) / 2;
	y = inputImage.rows - cropRealSize.height;

	outputImage = inputImage( cv::Rect( x, y, cropRealSize.width, cropRealSize.height ) );

	return 0;
}
