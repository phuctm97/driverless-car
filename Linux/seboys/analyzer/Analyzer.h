#ifndef __SB_ANALYZER_H__
#define __SB_ANALYZER_H__

#include "../Params.h"
#include "../calculator/FrameInfo.h"
#include "RoadInfo.h"

class Analyzer {
private:

public:
    Analyzer() {}

    int init(const sb::Params &params);

    int analyze(const sb::FrameInfo &frameInfo,
                sb::RoadInfo &roadInfo) const;

    void release();
};

#endif //!__SB_ANALYZER_H__
