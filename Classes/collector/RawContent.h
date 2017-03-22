#ifndef __SB_RAW_CONTENT_H__
#define __SB_RAW_CONTENT_H__

#include "../Include.h"

namespace sb
{
class RawContent
{
private:
	cv::Mat _colorImage;
	cv::Mat _depthImage;

public:
	RawContent() {}

	const cv::Mat& getColorImage() const;

	void setColorImage( const cv::Mat& colorImage );

	const cv::Mat& getDepthImage() const;

	void setDepthImage( const cv::Mat& depthImage );
};
}

#endif //!__SB_RAW_CONTENT_H__
