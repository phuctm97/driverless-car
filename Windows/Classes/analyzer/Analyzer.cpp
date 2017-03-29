#include "Analyzer.h"
#include "../collector/RawContent.h"

int sb::Analyzer::init( const sb::Params& params )
{
	return 0;
}

int sb::Analyzer::analyze( const sb::FrameInfo& frameInfo, sb::RoadInfo& roadInfo ) const
{
	const int N_LINES = static_cast<int>(frameInfo.getWarpedLines().size());

	std::vector<float> _lineRatings( N_LINES, 0 );

	for ( int i = 0; i < N_LINES; i++ ) {
		const sb::LineInfo& line = frameInfo.getLines()[i];
		const sb::LineInfo& warpedLine = frameInfo.getWarpedLines()[i];

		_lineRatings[i] += warpedLine.getLength();
		_lineRatings[i] += 5.0 * (line.getAverageColor()[0] + line.getAverageColor()[1] + line.getAverageColor()[2]) / 3;
	}

	///// Debug /////

	// create warped image
	int H = 400, W = 1800;
	cv::Mat warpedImage( frameInfo.getColorImage().rows + H,
	                     frameInfo.getColorImage().cols + W, CV_8UC3, cv::Scalar( 0, 0, 0 ) );

	for ( const auto& line : frameInfo.getWarpedLines() ) {
		cv::line( warpedImage,
		          line.getStartingPoint() + cv::Point2d( W / 2, H ),
		          line.getEndingPoint() + cv::Point2d( W / 2, H ),
		          cv::Scalar( 0, 0, 255 ), 1 );
	}

	// debug each lines
	for ( int i = 0; i < N_LINES; i++ ) {
		const sb::LineInfo& line = frameInfo.getLines()[i];
		const sb::LineInfo& warpedLine = frameInfo.getWarpedLines()[i];

		cv::Mat tempImage = warpedImage.clone();
		cv::line( tempImage,
		          warpedLine.getStartingPoint() + cv::Point2d( W / 2, H ),
		          warpedLine.getEndingPoint() + cv::Point2d( W / 2, H ),
		          cv::Scalar( 0, 255, 0 ), 2 );

		std::stringstream stringBuilder;

		stringBuilder << "Rating: " << _lineRatings[i];
		cv::putText( tempImage,
		             stringBuilder.str(),
		             cv::Point( 20, 15 ),
		             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

		cv::imshow( "Test2", tempImage );
		cv::waitKey();
	}

	return 0;
}

void sb::Analyzer::release() { }

int sb::Analyzer::analyze1( const sb::FrameInfo& frameInfo, sb::RoadInfo& roadInfo ) const
{
	const double FRAME_HALF_WIDTH = frameInfo.getColorImage().cols / 2;

	const double OLD_ROTATION = roadInfo.getRotationOfLanes()[0];
	const double OLD_POSITION_X_OF_LEFT_LANE = roadInfo.getPositionOfLeftLane();
	const double OLD_POSITION_X_OF_RIGHT_LANE = roadInfo.getPositionOfRightLane();

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

		if ( abs( rotation - OLD_ROTATION ) > 20 ) continue;

		if ( positionX < 0 ) {
			if ( abs( positionX - OLD_POSITION_X_OF_LEFT_LANE ) > 0.2 ) continue;;

			sumRotation += rotation;
			n1++;

			sumPositionXOfLeftLane += positionX;
			n2++;
		}
		else {
			if ( abs( positionX - OLD_POSITION_X_OF_RIGHT_LANE ) > 0.2 )continue;

			sumRotation += rotation;
			n1++;

			sumPositionXOfRightLane += positionX;
			n3++;
		}
	}

	double rotation = sumRotation / n1;
	double positionXOfLeftLane = sumPositionXOfLeftLane / n2;
	double positionXOfRightLane = sumPositionXOfRightLane / n3;

	roadInfo.setRotationOfLanes( std::vector<double>( 1, rotation ) );
	roadInfo.setPositionOfLeftLane( positionXOfLeftLane );
	roadInfo.setPositionOfRightLane( positionXOfRightLane );

	return 0;
}
