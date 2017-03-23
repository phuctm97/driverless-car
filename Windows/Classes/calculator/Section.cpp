#include "Section.h"

int sb::Section::getIndexInContainer() const { return _indexInContainer; }

const cv::Rect2d& sb::Section::getAreaInContainer() const { return _areaInContainer; }

const cv::Mat& sb::Section::getDataImage() const { return _dataImage; }

const std::vector<sb::Line>& sb::Section::getLines() const { return _lines; }

std::vector<sb::Line>& sb::Section::getLines() { return _lines; }

sb::Line sb::Section::getBottomLine() const
{
	return sb::Line( _areaInContainer.br(),
	                 _areaInContainer.br() + cv::Point2d( 1, 0 ) );
}

sb::Line sb::Section::getTopLine() const
{
	return sb::Line( _areaInContainer.tl(),
	                 _areaInContainer.tl() + cv::Point2d( 1, 0 ) );
}

cv::Point2d sb::Section::convertToContainerSpace( const cv::Point2d& pos ) const
{
	return cv::Point2d( pos.x, pos.y + _areaInContainer.y );
}
