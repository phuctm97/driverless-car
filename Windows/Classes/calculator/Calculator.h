#ifndef __SB_CALCULATOR_H__
#define __SB_CALCULATOR_H__

#include "../Params.h"
#include "../Timer.h"
#include "../collector/RawContent.h"
#include "FrameInfo.h"
#include "Formatter.h"
#include "EdgeDetector.h"
#include "LineDetector.h"

namespace sb
{
struct Calculator
{
	cv::Rect cropBox;
	std::vector<cv::Rect> splitBoxes;
};

int init( sb::Calculator* calculator, sb::Params* params );

int calculate( sb::Calculator* calculator,
               sb::RawContent* rawContent,
               sb::FrameInfo* frameInfo );

void release( sb::Calculator* calculator );

void findBlobs( const cv::Mat& binary, std::vector<sb::Blob*> &blobs );

}

#endif //!__SB_CALCULATOR_H__
