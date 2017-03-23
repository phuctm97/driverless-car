#ifndef __SB_FORMATTER_H__
#define __SB_FORMATTER_H__

#include "Line.h"
#include "LineInfo.h"

class Formatter {
private:
    cv::Rect _cropBox;

    cv::Point2f _warpSourceQuad[4];

    cv::Point2f _warpDestinationQuad[4];

public:
    Formatter() {}

    Formatter(const cv::Rect &cropBox,
              const cv::Point2f *warpOriginalSourceQuad,
              const cv::Point2f *warpOriginalDestinationQuad);

    int crop(const cv::Mat &inputImage, cv::Mat &outputImage) const;

    int warp(const std::vector<sb::LineInfo> originalLines,
             std::vector<sb::LineInfo> &outputLines) const;
};


#endif //!__SB_FORMATTER_H__
