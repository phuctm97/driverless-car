#ifndef __SB_FRAME_INFO_H__
#define __SB_FRAME_INFO_H__

#include "Formatter.h"
#include "EdgeDetector.h"
#include "LineDetector.h"
#include "../Params.h"

namespace sb
{
class FrameInfo
{
private:
	double _convertCoordCoef;

	cv::Mat _colorImage;

	std::vector<sb::LineInfo> _imageLineInfos;

	std::vector<sb::LineInfo> _realLineInfos;

	std::vector<sb::SectionInfo> _sectionInfos;

public:
	FrameInfo()
		: _convertCoordCoef( 1.0 ) {}

	void create( const sb::Params& params );

	const cv::Mat& getColorImage() const;

	void setColorImage( const cv::Mat& colorImage );

	const std::vector<sb::LineInfo>& getImageLineInfos() const;

	void setImageLineInfos( const std::vector<sb::LineInfo>& lineInfos );

	const std::vector<sb::LineInfo>& getRealLineInfos() const;

	void setRealLineInfos( const std::vector<sb::LineInfo>& warpedLines );

	const std::vector<sb::SectionInfo>& getSectionInfos() const;

	void setSectionInfos( const std::vector<sb::SectionInfo>& sectionInfos );

	/*double convertXToCoord( double x ) const;

	double convertYToCoord( double y ) const;

	cv::Point2d convertToCoord( const cv::Point2d& point ) const;

	double convertXFromCoord( double x ) const;

	double convertYFromCoord( double y ) const;

	cv::Point2d convertFromCoord( const cv::Point2d& point ) const;

	double convertToRotation( double angle ) const;

	double convertFromRotation( double rotation ) const;*/
};
}

#endif //!__SB_FRAME_INFO_H__

