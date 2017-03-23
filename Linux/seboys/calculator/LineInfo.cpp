#include "LineInfo.h"

const sb::Line& sb::LineInfo::getLine() const { return _line; }

const cv::Point2d& sb::LineInfo::getStartingPoint() const { return _line.getStartingPoint(); }

const cv::Point2d& sb::LineInfo::getEndingPoint() const { return _line.getEndingPoint(); }

double sb::LineInfo::getLength() const { return _length; }

double sb::LineInfo::getAngle() const { return _angle; }
