#ifndef __SB_RAW_CONTENT_H__
#define __SB_RAW_CONTENT_H__

#include "../Params.h"

namespace sb
{

class RawContent
{
private:
	cv::Mat _colorImage;

public:
	RawContent() {}

	void create( const sb::Params& params );

	const cv::Mat& getColorImage() const;

	void setColorImage( const cv::Mat& colorImage );
};

}

#endif //!__SB_RAW_CONTENT_H__
