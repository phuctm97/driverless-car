#ifndef __SB_CALCULATOR_H__
#define __SB_CALCULATOR_H__

#include "../Params.h"
#include "../Timer.h"
#include "../collector/RawContent.h"
#include "FrameInfo.h"
#include "EdgeDetector.h"

#define MIN_ACCEPTABLE_BLOB_OBJECTS_COUNT 20

namespace sb
{
struct Calculator
{
	sb::EdgeDetector edgeDetector;

	double binarizeThesh;

	cv::Rect cropBox;

	std::vector<cv::Rect> splitBoxes;
};

int init( sb::Calculator* calculator, sb::Params* params );

int calculate( sb::Calculator* calculator,
               sb::RawContent* rawContent,
               sb::FrameInfo* frameInfo );

void release( sb::Calculator* calculator );

void findBlobs( sb::Calculator* calculator, sb::FrameInfo* frameInfo );

}

#endif //!__SB_CALCULATOR_H__

