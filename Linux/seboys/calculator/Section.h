#ifndef __SB_SECTION_H__
#define __SB_SECTION_H__

#include "Line.h"

class Section {
private:
    // index in container-image
    int _indexInContainer;

    // rect in container-image
    cv::Rect2d _areaInContainer;

    // data source (an area in the container-image)
    cv::Mat _dataImage;

    // detected lines
    std::vector<sb::Line> _lines;

public:
    explicit Section(int indexInContainer,
                     const cv::Rect2d &areaInContainer,
                     const cv::Mat &containerImage)
            : _indexInContainer(indexInContainer),
              _areaInContainer(areaInContainer),
              _dataImage(containerImage, areaInContainer) {}

    int getIndexInContainer() const;

    const cv::Rect2d &getAreaInContainer() const;

    const cv::Mat &getDataImage() const;

    const std::vector<sb::Line> &getLines() const;

    std::vector<sb::Line> &getLines();

    sb::Line getBottomLine() const;

    sb::Line getTopLine() const;

    cv::Point2d convertToContainerSpace(const cv::Point2d &pos) const;
};


#endif //!__SB_SECTION_H__
