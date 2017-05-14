#include "CropTool.h"

int sb::CropTool::crop( const cv::Mat& inputImage, cv::Mat& outputImage )
{
	if ( _cropBox.x < 0 || _cropBox.y < 0 ||
		_cropBox.x + _cropBox.width > inputImage.cols ||
		_cropBox.y + _cropBox.height > inputImage.rows ) {
		std::cerr << "CropTool: Input image and crop box aren't suitable." << std::endl;
		return -1;
	}

	outputImage = inputImage( _cropBox );
	return 0;
}
