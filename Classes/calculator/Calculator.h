#ifndef __SB_CALCULATOR_H__
#define __SB_CALCULATOR_H__

#include "../Params.h"
#include "../collector/RawContent.h"
#include "FrameInfo.h"

namespace sb
{
class Calculator
{
private:
	sb::Formatter _formatter;
	sb::EdgeDetector _edgeDetector;
	sb::LineDetector _lineDetector;

public:
	Calculator() {};

	int init( const sb::Params& params );

	int calculate( const sb::RawContent& rawContent,
	               sb::FrameInfo& frameInfo ) const;

	void release();
};
}

#endif //!__SB_CALCULATOR_H__
