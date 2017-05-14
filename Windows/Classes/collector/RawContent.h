#ifndef __SB_RAW_CONTENT_H__
#define __SB_RAW_CONTENT_H__

#include "../Params.h"

namespace sb
{
<<<<<<< HEAD
=======

class RawContent
{
private:
	cv::Mat _colorImage;
>>>>>>> master

struct RawContent
{
	cv::Mat colorImage;
};

void create( sb::RawContent* rawContent, sb::Params* params );

void release( sb::RawContent* rawContent );

<<<<<<< HEAD
=======
	void setColorImage( const cv::Mat& colorImage );
};

>>>>>>> master
}

#endif //!__SB_RAW_CONTENT_H__
