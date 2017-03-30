#ifndef __SB_FORMATTER_H__
#define __SB_FORMATTER_H__

#include "../Include.h"
#include "LineInfo.h"
#include "SectionInfo.h"

namespace sb
{
class Formatter
{
private:
	cv::Rect _cropBox;

	cv::Matx33f _warpMatrix;

	double _convertCoordCoef;

	std::vector<int> _separateRows;

public:
	Formatter() {}

	Formatter( const cv::Rect& cropBox,
	           const cv::Point2f* warpOriginalSourceQuad,
	           const cv::Point2f* warpOriginalDestinationQuad,
	           double convertCoordCoef,
	           const std::vector<int>& separateRows );

	int crop( const cv::Mat& inputImage, cv::Mat& outputImage ) const;

	int warp( const std::vector<sb::LineInfo> originalLines,
	          std::vector<sb::LineInfo>& outputLines ) const;

	int split( const std::vector<sb::LineInfo> warpedLines,
	           int containerHeight,
	           std::vector<sb::SectionInfo>& outputSections ) const;

	double convertXToCoord( double x ) const;

	double convertYToCoord( double y ) const;

	cv::Point2d convertToCoord( const cv::Point2d& point ) const;

	double convertXFromCoord( double x ) const;

	double convertYFromCoord( double y ) const;

	cv::Point2d convertFromCoord( const cv::Point2d& point ) const;

	double convertToRotation( double angle ) const;

	double convertFromRotation( double rotation ) const;
};
}

#endif //!__SB_FORMATTER_H__
