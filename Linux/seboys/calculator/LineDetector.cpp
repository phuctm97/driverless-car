#include "LineDetector.h"

void LineDetector::apply(const cv::Mat &binaryImage,
                         std::vector<Line> &lines) const {
    // reset array
    lines.clear();

    // apply Hough transform
    std::vector<cv::Vec4i> houghLinesPOutput;
    cv::HoughLinesP(binaryImage,
                    houghLinesPOutput,
                    _houghLinesPRho,
                    _houghLinesPTheta,
                    _houghLinesPThreshold,
                    _houghLinesPMinLineLength,
                    _houghLinesPMaxLineGap);

    // generate Line and push to the array
    for (auto vec : houghLinesPOutput) {

        Line line(cv::Point2d(vec[0], vec[1]),
                      cv::Point2d(vec[2], vec[3]));

        if (!line.isValid()) continue;

        lines.push_back(line);

    }
}

void LineDetector::apply(const cv::Mat &binaryImage, std::vector<LineInfo> &lines) const {
    // reset array
    lines.clear();

    // apply Hough transform
    std::vector<cv::Vec4i> houghLinesPOutput;
    cv::HoughLinesP(binaryImage,
                    houghLinesPOutput,
                    _houghLinesPRho,
                    _houghLinesPTheta,
                    _houghLinesPThreshold,
                    _houghLinesPMinLineLength,
                    _houghLinesPMaxLineGap);

    // generate Line and push to the array
    for (auto vec : houghLinesPOutput) {

        Line line(cv::Point2d(vec[0], vec[1]),
                      cv::Point2d(vec[2], vec[3]));

        if (!line.isValid()) continue;

        lines.push_back(LineInfo(line));

    }
}
