#include "LineInfo.h"

sb::LineInfo::LineInfo( const sb::Line& line,
                        const cv::Vec3b& averageColor )
	: _line( line ),
	  _angle( line.getAngleWithOx() ),
	  _length( line.getLength() ),
	  _middlePoint( (line.getStartingPoint() + line.getEndingPoint()) * 0.5 ),
	  _averageColor( averageColor ) {}

const sb::Line& sb::LineInfo::getLine() const { return _line; }

const cv::Point2d& sb::LineInfo::getStartingPoint() const { return _line.getStartingPoint(); }

const cv::Point2d& sb::LineInfo::getEndingPoint() const { return _line.getEndingPoint(); }

double sb::LineInfo::getLength() const { return _length; }

double sb::LineInfo::getAngle() const { return _angle; }

const cv::Vec3b& sb::LineInfo::getAverageColor() const { return _averageColor; }

const cv::Point2d& sb::LineInfo::getMiddlePoint() const { return _middlePoint; }

void sb::LineInfo::setLine( const sb::Line& line )
{
	_line = line;
	_length = line.getLength();
	_angle = line.getAngleWithOx();
}

void sb::LineInfo::setAverageColor( const cv::Vec3b& averageColor ) { _averageColor = averageColor; }
