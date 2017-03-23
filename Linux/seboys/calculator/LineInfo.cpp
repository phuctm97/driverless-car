#include "LineInfo.h"

const Line &LineInfo::getLine() const { return _line; }

const cv::Point2d &LineInfo::getStartingPoint() const { return _line.getStartingPoint(); }

const cv::Point2d &LineInfo::getEndingPoint() const { return _line.getEndingPoint(); }

double LineInfo::getLength() const { return _length; }

double LineInfo::getAngle() const { return _angle; }
