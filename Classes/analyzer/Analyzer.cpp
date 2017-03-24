#include "Analyzer.h"
#include "../collector/RawContent.h"

int sb::Analyzer::init( const sb::Params& params )
{
	return 0;
}

int sb::Analyzer::analyze( const sb::FrameInfo& frameInfo, sb::RoadInfo& roadInfo ) const
{
	const double FRAME_HALF_WIDTH = frameInfo.getColorImage().cols / 2;

	const double OLD_ROTATION = roadInfo.getRotationOfLane();
	const double OLD_POSITION_X_OF_LEFT_LANE = roadInfo.getPositionOfLeftLane().x;
	const double OLD_POSITION_X_OF_RIGHT_LANE = roadInfo.getPositionOfRightLane().x;

	double sumRotation = 0;
	double sumPositionXOfLeftLane = 0;
	double sumPositionXOfRightLane = 0;

	int n1 = 0;
	int n2 = 0;
	int n3 = 0;

	for ( int i = 0; i < static_cast<int>(frameInfo.getWarpedLines().size()); i++ ) {

		const sb::LineInfo& line = frameInfo.getWarpedLines()[i];

		double rotation = 90 - line.getAngle();
		double positionX = line.getEndingPoint().x / FRAME_HALF_WIDTH - 1;

		if ( abs( rotation - OLD_ROTATION ) > 15 ) continue;

		if( positionX < 0 ) {
			if( abs( positionX - OLD_POSITION_X_OF_LEFT_LANE ) > 0.2 ) continue;;

			sumRotation += rotation;
			n1++;

			sumPositionXOfLeftLane += positionX;
			n2++;

		}
		else {
			if( abs( positionX - OLD_POSITION_X_OF_RIGHT_LANE ) > 0.2 )continue;

			sumRotation += rotation;
			n1++;

			sumPositionXOfRightLane += positionX;
			n3++;

		}
	}

	double rotation = sumRotation / n1;
	double positionXOfLeftLane = sumPositionXOfLeftLane / n2;
	double positionXOfRightLane = sumPositionXOfRightLane / n3;

	roadInfo.setRotationOfLane( rotation );
	roadInfo.setPositionOfLeftLane( cv::Point2d( positionXOfLeftLane, 0 ) );
	roadInfo.setPositionOfLeftLane( cv::Point2d( positionXOfRightLane, 0 ) );

	return 0;
}

void sb::Analyzer::release() {}
