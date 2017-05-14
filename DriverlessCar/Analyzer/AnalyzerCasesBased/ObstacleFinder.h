#ifndef __SB_OBSTACLE_FINDER_H__
#define __SB_OBSTACLE_FINDER_H__

#include <opencv2/opencv.hpp>

namespace sb
{
class ObstacleFinder
{
public:
	int init();

	int checkObstacle( const cv::Mat& image );

	void release();
};
}

#endif //!__SB_OBSTACLE_FINDER_H__