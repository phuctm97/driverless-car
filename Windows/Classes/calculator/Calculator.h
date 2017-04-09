#ifndef __SB_CALCULATOR_H__
#define __SB_CALCULATOR_H__

#include "../Params.h"
#include "../collector/RawContent.h"
#include "FrameInfo.h"
#include "Formatter.h"
#include "EdgeDetector.h"
#include "LineDetector.h"

namespace sb
{
class Calculator
{
private:
	sb::EdgeDetector _edgeDetector;
	sb::LineDetector _lineDetector;

	cv::Rect _cropBox;
	std::vector<cv::Rect> _splitBoxes;

public:
	Calculator() {};

	int init( const sb::Params& params );

	int calculate( const sb::RawContent& rawContent,
	               sb::FrameInfo& frameInfo ) const;

	void release();
};
}

#endif //!__SB_CALCULATOR_H__

