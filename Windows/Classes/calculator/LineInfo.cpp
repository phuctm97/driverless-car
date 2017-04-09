#include "LineInfo.h"

sb::LineInfo::LineInfo( const sb::Line& line )
	: _line( line ),
	  _angle( line.getAngleWithOx() ),
	  _length( line.getLength() ),
	  _middlePoint( (line.getStartingPoint() + line.getEndingPoint()) * 0.5 ) {}

const sb::Line& sb::LineInfo::getLine() const { return _line; }

const cv::Point2d& sb::LineInfo::getStartingPoint() const { return _line.getStartingPoint(); }

const cv::Point2d& sb::LineInfo::getEndingPoint() const { return _line.getEndingPoint(); }

double sb::LineInfo::getLength() const { return _length; }

double sb::LineInfo::getAngle() const { return _angle; }

const cv::Point2d& sb::LineInfo::getMiddlePoint() const { return _middlePoint; }

const cv::Point2d& sb::LineInfo::getTopPoint() const { return _topPoint; }

void sb::LineInfo::setTopPoint( const cv::Point2d& topPoint ) { _topPoint = topPoint; }

const cv::Point2d& sb::LineInfo::getBottomPoint() const { return _bottomPoint; }

void sb::LineInfo::setBottomPoint( const cv::Point2d& bottomPoint ) { _bottomPoint = bottomPoint; }

const cv::Point2d& sb::LineInfo::getCenterPoint() const { return _centerPoint; }

void sb::LineInfo::setCenterPoint( const cv::Point2d& centerPoint ) { _centerPoint = centerPoint; }

