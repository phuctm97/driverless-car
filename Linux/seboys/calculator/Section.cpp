#include "Section.h"

int Section::getIndexInContainer() const { return _indexInContainer; }

const cv::Rect2d &Section::getAreaInContainer() const { return _areaInContainer; }

const cv::Mat &Section::getDataImage() const { return _dataImage; }

const std::vector<Line> &Section::getLines() const { return _lines; }

std::vector<Line> &Section::getLines() { return _lines; }

Line Section::getBottomLine() const {
    return Line(_areaInContainer.br(),
                _areaInContainer.br() + cv::Point2d(1, 0));
}

Line Section::getTopLine() const {
    return Line(_areaInContainer.tl(),
                _areaInContainer.tl() + cv::Point2d(1, 0));
}

cv::Point2d Section::convertToContainerSpace(const cv::Point2d &pos) const {
    return cv::Point2d(pos.x, pos.y + _areaInContainer.y);
}
