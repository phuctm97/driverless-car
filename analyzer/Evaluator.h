#ifndef __SB_EVALUATOR_H__
#define __SB_EVALUATOR_H__

#include "Line.h"

#define ANGLE_CONSIDERED_AS_HORIZONTAL 15
#define ANGLE_CONSIDERED_AS_VERTICAL 75

namespace sb
{
class Evaluator
{
private:
	int _tendency = 0;
	int _direction = 0;

	double _alpha = 60;

public:
	void staticEvaluate( sb::Line& line ) const;

private:
	float normalize( float x, float min, float max );
};
}

#endif //!__SB_EVALUATOR_H__
