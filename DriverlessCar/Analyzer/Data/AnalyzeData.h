#ifndef __SB_ANALYZE_DATA_H__
#define __SB_ANALYZE_DATA_H__

#include <opencv2/opencv.hpp>

namespace sb
{

struct AnalyzeData
{
	int state;

	cv::Point target;
};

void release( AnalyzeData* data );

}

#endif //!__SB_ANALYZE_DATA_H__
