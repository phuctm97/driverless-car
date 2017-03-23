#include "FrameInfo.h"

FrameInfo::FrameInfo() {}

int FrameInfo::create(const cv::Mat &colorImage,
                          const cv::Mat &depthImage,
                          const Formatter &formatter,
                          const EdgeDetector &edgeDetector,
                          const LineDetector &lineDetector) {
    // 1) reference to new frame
    _colorImage.release();

    if (formatter.crop(colorImage, _colorImage) < 0) {
        std::cerr << "Crop image failed." << std::endl;
        return -1;
    }

    _depthImage.release();

    _depthImage = depthImage;

    // 2) generate edges-frame
    cv::Mat edgesFrame;

    cv::cvtColor(_colorImage, edgesFrame, cv::COLOR_BGR2GRAY);

    edgeDetector.apply(edgesFrame);

    // 3) generate lines in whole frame
    lineDetector.apply(edgesFrame, _lines);

    // 4) generate warped lines;
    if (formatter.warp(_lines, _warpedLines) < 0) {
        std::cerr << "Warp lines failed." << std::endl;
        return -1;
    }

    return 0;
}

const cv::Mat &FrameInfo::getColorImage() const { return _colorImage; }

const cv::Mat &FrameInfo::getDepthImage() const { return _depthImage; }

const std::vector<LineInfo> &FrameInfo::getLines() const { return _lines; }

const std::vector<LineInfo> &FrameInfo::getWarpedLines() const { return _warpedLines; }