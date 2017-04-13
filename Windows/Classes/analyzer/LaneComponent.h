#ifndef __SB_LANE_COMPONENT_H__
#define __SB_LANE_COMPONENT_H__

#include "../calculator/FrameInfo.h"
#include "LanePart.h"

#define MAX_ACCEPTABLE_COLOR_ERROR_TO_WHITE 20

#define MAX_ACCEPTABLE_POSITION_DIFF_IN_LANE_PARTS 70
#define MAX_ACCEPTABLE_COLOR_DIFF_IN_LANE_PARTS 20
#define MAX_ACCEPTABLE_ANGLE_DIFF_IN_LANE_PARTS 20

#define MAX_ACCEPTABLE_POSITION_ERROR_IN_TRACK_LANE_PART 70
#define MAX_ACCEPTABLE_COLOR_ERROR_IN_TRACK_LANE_PART 10

namespace sb
{

enum LaneError
{
	NICE_LANE,
	DASHED_LANE,
	OUTSIGHT_LANE,
	OVERLAYED_LANE
};

struct LaneComponent
{
	int side;

	int minLaneWidth;

	int maxLaneWidth;

	std::vector<sb::LanePartInfo*> parts;

	int errorCode;
};

void release( sb::LaneComponent* laneComponent );

void init( sb::LaneComponent* laneComponent, int side );

int find( sb::LaneComponent* laneComponent, sb::FrameInfo* frameInfo );

void findFirstLaneParts( sb::LaneComponent* laneComponent, sb::FrameInfo* frameInfo, std::vector<sb::LanePart*>& firstLaneParts );

void findBestLaneParts( sb::LaneComponent* laneComponent, sb::FrameInfo* frameInfo,
                        sb::LanePart* firstLanePart,
                        std::vector<sb::LanePart*>& fullLaneParts, float& fullLaneRating );

void findNextLaneParts( sb::LaneComponent* laneComponent, sb::FrameInfo* frameInfo,
                        sb::Section* section, sb::LanePart* lastestLanePart,
                        std::vector<std::pair<sb::LanePart*, float>>& nextLaneParts );

int track( sb::LaneComponent* laneComponent, sb::FrameInfo* frameInfo );

void trackIndividualPart( sb::LaneComponent* laneComponent, sb::FrameInfo* frameInfo,
                          sb::Section* section, sb::LanePartInfo* oldPartInfo,
                          std::vector<sb::LanePartInfo*>& trackResults );

double calculateDeltaE( const cv::Vec3f& bgr1, const cv::Vec3f& bgr2, double kL = 1, double kC = 1, double kH = 1 );

cv::Vec3f cvtBGRtoLab( const cv::Vec3b& bgr );
}

#endif //!__SB_LANE_COMPONENT_H__
