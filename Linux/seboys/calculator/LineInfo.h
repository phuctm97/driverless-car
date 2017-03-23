#ifndef __SB_LINE_INFO_H__
#define __SB_LINE_INFO_H__

#include "Line.h"

class LineInfo {
private:
    sb::Line _line;

    double _length;

    double _angle;

public:

    LineInfo(const sb::Line &line)
            : _line(line),
              _angle(line.getAngleWithOx()),
              _length(line.getLength()) {}

    const sb::Line &getLine() const;

    const cv::Point2d &getStartingPoint() const;

    const cv::Point2d &getEndingPoint() const;

    double getLength() const;

    double getAngle() const;
};

#endif //!__SB_LINE_INFO_H__
