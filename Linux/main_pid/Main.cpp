#pragma region OpenCV References
// OpenCV References
#include <opencv2/opencv.hpp>

#pragma endregion

#pragma region STD References
// STD References
#include <ctime>
#include <device/MyDevive.h>
#include <device/util.h>

#pragma endregion

#pragma region SB References
// SB References
#include "car/EdgeDetector.h"
#include "car/JoinerSplitter.h"
#include "car/LineDetector.h"
#include "car/BoundariesJudge.h"
#include "car/CarController.h"

#pragma endregion


int main(const int argc, const char *argv[]) {
    // A.0) Params
    cv::String VIDEO_PATH;
    cv::String HYPOS_PATH;
    cv::String PARAMS_PATH;
    parseCommands(argc, argv, VIDEO_PATH, HYPOS_PATH, PARAMS_PATH);

    // A.a) Image stream, can be obtained from either video file (for test) or camera (for real-time working)
    MyDevice *myDevice = new MyDevice();
    myDevice->init();

    // A.b) Current frame loaded from stream
    cv::Mat currentFrame;
    cv::Mat workingFrame;

    // A.c) Sections splitted from current frame
    std::vector<sb::Section> currentSections;

    // A.d) Controller parameters
    int velocity = 0;
    int theta = 0;

    // B.a) Initial parameters (static parameters)
    sb::Params params(PARAMS_PATH, /*minX*/ 0, /*maxX*/ VIDEO_FRAME_WIDTH - 1);

    // B.b) Hypothesises for detecting (dynamic parameters)
    sb::Hypos hypos(HYPOS_PATH, &params);

    // C) Frame joiner-splitter
    sb::JoinerSplitter joinerSplitter(&hypos);

    // D) Edge detector
    sb::EdgeDetector edgeDetector(&params);

    // E) Line detector
    sb::LineDetector lineDetector(&hypos, &params);

    // F) BoundariesJudge
    sb::BoundariesJudge boundariesJudge(&hypos, &params);

    // H) CarController
    CarController controller(velocity);

    // 0) Initial Pause
    cv::waitKey();
    controller.start();

    while (true) {

        // 1) load frame and generate binarized sections
        if (!loadBinarizedSections(myDevice,
                                   currentFrame,
                                   workingFrame,
                                   currentSections,
                                   hypos,
                                   joinerSplitter,
                                   edgeDetector))
            break;

        //*[DEBUG]*/ showEdges( workingFrame );

        // 2) line detect
        lineDetector.apply(currentSections);

        //*[DEBUG]*/ showLines( workingFrame, currentSections, hypos );

        // 3) boundaries judge
        boundariesJudge.apply(currentSections);

        /*[DEBUG]*/
        showLines(workingFrame, currentSections, hypos);

        // 4) update left/right line, update hypothesises
        updateBoundaries(currentSections, hypos, boundariesJudge);

        //*[DEBUG]*/ showResult( currentFrame, currentSections, hypos );

        if (hypos.getTendency() < 0) theta += 1;
        else if (hypos.getTendency() > 0)theta -= 1;

        controller.changeSpeed(velocity);
        controller.changeTheta(theta);
        controller.update();

        if (cv::waitKey(16) == KEY_TO_ESCAPE)break;
    }

    return 0;
}
