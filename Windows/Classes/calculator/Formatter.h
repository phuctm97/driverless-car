#ifndef __SB_FORMATTER_H__
#define __SB_FORMATTER_H__

#include "../Include.h"
#include "LineInfo.h"

namespace sb
{
class Formatter
{
private:
	cv::Rect _cropBox;

	cv::Matx33f _warpMatrix;

	double _convertCoordCoef;

public:
	Formatter()
		: _convertCoordCoef( 0 ) {}

	Formatter( const cv::Rect& cropBox,
	           const cv::Point2f* warpOriginalSourceQuad,
	           const cv::Point2f* warpOriginalDestinationQuad,
	           double convertCoordCoef );

	int crop( const cv::Mat& inputImage, cv::Mat& outputImage ) const;

	int warp( const std::vector<sb::LineInfo*> imageLines,
	          std::vector<sb::LineInfo*>& outputRealLines ) const;

	double convertXToCoord( double x ) const;

	double convertYToCoord( double y ) const;

	double convertLengthToCoord( double length ) const;

	double convertLengthFromCoord( double length ) const;

	cv::Point2d convertToCoord( const cv::Point2d& point ) const;

	cv::Rect2d convertToCoord( const cv::Rect2d& rect ) const;

	double convertXFromCoord( double x ) const;

	double convertYFromCoord( double y ) const;

	cv::Point2d convertFromCoord( const cv::Point2d& point ) const;

	cv::Rect2d convertFromCoord( const cv::Rect2d& rect ) const;
};
}

#endif //!__SB_FORMATTER_H__

