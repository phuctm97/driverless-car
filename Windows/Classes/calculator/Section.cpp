#include "Section.h"

sb::Section::Section( const cv::Mat& containerBinaryImage,
                      const cv::Rect& rect )
{
	_binaryImage = containerBinaryImage( rect );
	_imageRect = rect;
}

const cv::Mat& sb::Section::getBinaryImage() const { return _binaryImage; }

void sb::Section::setBinaryImage( const cv::Mat& binaryImage ) { _binaryImage = binaryImage; }

const cv::Rect2d& sb::Section::getImageRect() const { return _imageRect; }

void sb::Section::setImageRect( const cv::Rect2d& imageRect ) { _imageRect = imageRect; }

const std::vector<sb::LineInfo>& sb::Section::getImageLines() const { return _imageLines; }

void sb::Section::setImageLines( const std::vector<sb::LineInfo>& imageLines ) { _imageLines = imageLines; }

sb::Line sb::Section::getBottomLine() const
{
	return sb::Line( cv::Point2d( 0, _imageRect.y + _imageRect.height - 1 ),
	                 cv::Point2d( 1, _imageRect.y + _imageRect.height - 1 ) );
}

sb::Line sb::Section::getTopLine() const
{
	return sb::Line( cv::Point2d( 0, _imageRect.y ),
	                 cv::Point2d( 1, _imageRect.y ) );
}

cv::Point2d sb::Section::convertToContainerSpace( const cv::Point2d& pos ) const
{
	return cv::Point2d( _imageRect.x + pos.x, pos.y + _imageRect.y );
}
