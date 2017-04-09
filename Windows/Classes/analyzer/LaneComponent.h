#ifndef __SB_LANE_COMPONENT_H__
#define __SB_LANE_COMPONENT_H__

#include "LanePart.h"
#include "../calculator/FrameInfo.h"

#include <stack>

#define SB_DEBUG

#define POSITION_THRESH_FOR_SIMILAR_LANE 2

#define MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_ADJACENT_LINES 20
#define MAX_ACCEPTABLE_DISTANCE_TO_WHITE_COLOR 145
#define MAX_ACCEPTABLE_DISTANCE_COLOR_BETWEEN_LINE 20

#define MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_ADJACENT_LANE_PARTS 20
#define MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_ADJACENT_LANE_PARTS 5
#define MAX_ACCEPTABLE_POSITION_DIFF_BETWEEN_ADJACENT_LANE_PARTS 5

#define MAX_ACCEPTABLE_ANGLE_ERROR_TRACK_LINE 10
#define MAX_ACCEPTABLE_POSITION_ERROR_TRACK_LINE 10
#define MAX_ACCEPTABLE_WIDTH_ERROR_TRACK_LINE 0.3

namespace sb
{
class LaneComponent
{
private:
	int _side;

	std::vector<sb::LanePartInfo> _parts;

	double _minLaneWidth;

	double _maxLaneWidth;

	double _windowWidth;

	double _windowMove;

public:
	void init( int side );

	int find( const sb::FrameInfo& frameInfo );

	int track( const sb::FrameInfo& frameInfo );

	const std::vector<sb::LanePartInfo>& getParts() const;

private:

	///// Find lane /////
	void findFirstLaneParts( const sb::Section& firstSection, const cv::Mat& colorImage, double windowX,
	                         std::vector<sb::LanePart>& firstLaneParts );

	void findNextLaneParts( const sb::Section& section, const cv::Mat& colorImage,
	                        const sb::LanePart& lastestLanePart,
	                        std::vector<std::pair<sb::LanePart, double>>& nextLaneParts );

	void findBestFullLaneParts( const std::vector<sb::Section>& sections, const cv::Mat& colorImage,
	                            const sb::LanePart& firstLanePart,
	                            std::vector<sb::LanePart>& fullLaneParts,
	                            double& fullLaneRating );

	///// Find lane /////

	///// Track lane /////

	int trackIndividualLanePart_PlanA( const sb::LanePart& part,
	                                   const sb::Section& section,
	                                   const cv::Mat& colorImage,
	                                   const cv::Mat& edgesImage,
	                                   std::vector<sb::LanePartInfo>& trackResults );

	int trackIndividualLanePart_PlanB( const sb::LanePart& part,
	                                   const sb::Section& section,
	                                   const cv::Mat& colorImage,
	                                   const cv::Mat& edgesImage,
	                                   std::vector<sb::LanePartInfo>& trackResults );

	int trackIndividualLanePart_PlanC( const sb::LanePart& part,
	                                   const sb::Section& section,
	                                   const cv::Mat& colorImage,
	                                   const cv::Mat& edgesImage,
	                                   std::vector<sb::LanePartInfo>& trackResults );

	int trackIndividualLanePart_PlanD( const sb::LanePart& part,
	                                   const sb::Section& section,
	                                   const cv::Mat& colorImage,
	                                   const cv::Mat& edgesImage,
	                                   std::vector<sb::LanePartInfo>& trackResults );

	void concludeIndividualLanePart( const std::vector<sb::LanePartInfo>& trackedLaneParts );

	///// Track lane /////

	void getPartColor( const cv::Mat& colorImage, sb::LanePart& part );

	cv::Vec3b getMainColor( const cv::Mat image, const cv::Point rectPoints[4] );

	double getColorDistance( const cv::Vec3b& color1, const cv::Vec3b& color2 );

#ifdef SB_DEBUG
	cv::Mat debugImages[2];

	void drawLanePart( cv::Mat& image, const sb::LanePart& part );
#endif
};
}

#endif
