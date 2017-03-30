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

	double convertXToCoord( double x ) const;

	double convertYToCoord( double y ) const;

	cv::Point2d convertToCoord( const cv::Point2d& point ) const;

	double convertXFromCoord( double x ) const;

	double convertYFromCoord( double y ) const;

	cv::Point2d convertFromCoord( const cv::Point2d& point ) const;

	double convertToRotation( double angle ) const;

	double convertFromRotation( double rotation ) const;

private:
	void calculateLineInfos( const std::vector<sb::Line>& lines,
													 const cv::Mat& colorImage,
													 std::vector<sb::LineInfo>& outputLineInfos ) const;
};
}

#endif //!__SB_CALCULATOR_H__
