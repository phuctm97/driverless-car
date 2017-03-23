#ifndef __SB_LINE_DETECTOR_H__
#define __SB_LINE_DETECTOR_H__

#include "LineInfo.h"

class LineDetector {
private:
    double _houghLinesPRho;
    double _houghLinesPTheta;
    int _houghLinesPThreshold;
    double _houghLinesPMinLineLength;
    double _houghLinesPMaxLineGap;

public:
    LineDetector() {}

    LineDetector(double houghLinesPRho,
                 double houghLinesPTheta,
                 int houghLinesPThreshold,
                 double houghLinesPMinLineLength,
                 double houghLinesPMaxLineGap)
            : _houghLinesPRho(houghLinesPRho),
              _houghLinesPTheta(houghLinesPTheta),
              _houghLinesPThreshold(houghLinesPThreshold),
              _houghLinesPMinLineLength(houghLinesPMinLineLength),
              _houghLinesPMaxLineGap(houghLinesPMaxLineGap) {}

    void apply(const cv::Mat &binaryImage,
               std::vector<sb::Line> &lines) const;

    void apply(const cv::Mat &binaryImage,
               std::vector<sb::LineInfo> &lines) const;
};

#endif //!__SB_LINE_DETECTOR_H__
