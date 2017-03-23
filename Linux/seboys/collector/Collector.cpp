#include "Collector.h"

int Collector::init(const Params &params) {
    // sample init

    _tempCap = cv::VideoCapture("../Debug/video-1.avi");

    if (!_tempCap.isOpened()) {
        std::cerr << "Init stream failed." << std::endl;
        return -1;
    }

    return 0;
}

int Collector::collect(RawContent &rawContent) {
    // sample collect

    cv::Mat colorImage;

    _tempCap >> colorImage;

    if (colorImage.empty()) {
        std::cerr << "Stream disconnected." << std::endl;
        return -1;
    }

    rawContent.setColorImage(colorImage);

    return 0;
}

void Collector::release() {
    // sample release

    _tempCap.release();

}
