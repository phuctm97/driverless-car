#ifndef __SB_FRAME_JOINER_SPLITTER_H__
#define __SB_FRAME_JOINER_SPLITTER_H__

#include "Section.h"

namespace sb
{
class JoinerSplitter
{
private:
	std::vector<float> _ratios;

public:
	explicit JoinerSplitter( const std::vector<float>& ratios )
		: _ratios( ratios ) {}

	void splitImageToSections( const cv::Mat& image,
	                           std::vector<sb::Section>& sections ) const;

	static void joinSectionsToImage( const std::vector<sb::Section>& sections,
	                                 cv::Mat& image );
};
}

#endif //!__SB_FRAME_JOINER_SPLITTER_H__
