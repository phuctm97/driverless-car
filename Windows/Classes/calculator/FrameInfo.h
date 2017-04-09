#ifndef __SB_FRAME_INFO_H__
#define __SB_FRAME_INFO_H__

#include "../Params.h"
#include "Section.h"

namespace sb
{
class FrameInfo
{
private:
	cv::Mat _colorImage;

	cv::Mat _edgesImage;

	std::vector<sb::Section> _imageSections;
public:
	FrameInfo() {}

	void create( const sb::Params& params );

	const cv::Mat& getColorImage() const;

	void setColorImage( const cv::Mat& colorImage );

	const std::vector<sb::Section>& getImageSections() const;

	void setImageSections( const std::vector<sb::Section>& imageSections );

	const cv::Mat& getEdgesImage() const;

	void setEdgesImage( const cv::Mat& edgesImage );
};
}

#endif //!__SB_FRAME_INFO_H__

