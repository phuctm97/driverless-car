#include "FrameInfo.h"

void sb::FrameInfo::create( const sb::Params& params )
{
	// _convertCoordCoef = 1.0 / (params.CROP_SIZE_WIDTH * params.COLOR_FRAME_SIZE.width * 0.5);
	_convertCoordCoef = 1.0; // params.CONVERT_COORD_COEF;
}

const cv::Mat& sb::FrameInfo::getColorImage() const { return _colorImage; }

void sb::FrameInfo::setColorImage( const cv::Mat& colorImage ) { _colorImage = colorImage; }

const std::vector<sb::LineInfo>& sb::FrameInfo::getImageLineInfos() const { return _imageLineInfos; }

void sb::FrameInfo::setImageLineInfos( const std::vector<sb::LineInfo>& lineInfos ) { _imageLineInfos = lineInfos; }

const std::vector<sb::LineInfo>& sb::FrameInfo::getRealLineInfos() const { return _realLineInfos; }

void sb::FrameInfo::setRealLineInfos( const std::vector<sb::LineInfo>& warpedLines ) { _realLineInfos = warpedLines; }

const std::vector<sb::SectionInfo>& sb::FrameInfo::getSectionInfos() const { return _sectionInfos; }

void sb::FrameInfo::setSectionInfos( const std::vector<sb::SectionInfo>& sectionInfos ) { _sectionInfos = sectionInfos; }

/*double sb::FrameInfo::convertXToCoord( double x ) const
{
	return (x - _colorImage.cols * 0.5) * _convertCoordCoef;
}

double sb::FrameInfo::convertYToCoord( double y ) const
{
	return (_colorImage.rows - y) * _convertCoordCoef;
}

cv::Point2d sb::FrameInfo::convertToCoord( const cv::Point2d& point ) const
{
	return cv::Point2d(
	                   convertXToCoord( point.x ),
	                   convertYToCoord( point.y )
	                  );
}

double sb::FrameInfo::convertXFromCoord( double x ) const
{
	return (x / _convertCoordCoef) + (_colorImage.cols * 0.5);
}

double sb::FrameInfo::convertYFromCoord( double y ) const
{
	return _colorImage.rows - (y / _convertCoordCoef);
}

cv::Point2d sb::FrameInfo::convertFromCoord( const cv::Point2d& point ) const
{
	return cv::Point2d(
	                   convertXFromCoord( point.x ),
	                   convertYFromCoord( point.x )
	                  );
}

double sb::FrameInfo::convertToRotation( double angle ) const
{
	return 90 - angle;
}

double sb::FrameInfo::convertFromRotation( double rotation ) const
{
	return 90 - rotation;
}*/

