#include "Analyzer.h"

int sb::init( sb::Analyzer* analyzer, sb::Params* params )
{
	analyzer->firstAnalyzeTimes = -1;

	analyzer->trackAnalyzeTimes = ANALYZER_TRACK_ANALYZE_TIMEOUT;

	analyzer->leftLane.init( -1 );

	analyzer->rightLane.init( 1 );

	return 0;
}

int sb::analyze( sb::Analyzer* analyzer,
								 sb::FrameInfo* frameInfo,
								 sb::RoadInfo* roadInfo )
{
	// first analyze
	if ( analyzer->firstAnalyzeTimes < ANALYZER_FIRST_ANALYZE_TIMEOUT ) {
		if ( firstAnalyze( frameInfo, roadInfo ) >= 0 ) {
			// success, stop first analyze, go to analyze track
			analyzer->firstAnalyzeTimes = ANALYZER_FIRST_ANALYZE_TIMEOUT;
			analyzer->trackAnalyzeTimes = -1;
			return 0;
		}

		// increase hops and check for timeout to stop system
		if ( ++analyzer->firstAnalyzeTimes >= ANALYZER_FIRST_ANALYZE_TIMEOUT ) return -1;
	}

	// track analyze
	if ( analyzer->trackAnalyzeTimes < ANALYZER_TRACK_ANALYZE_TIMEOUT ) {
		if ( trackAnalyze( frameInfo, roadInfo ) >= 0 ) {
			// success, to next frame
			analyzer->trackAnalyzeTimes = -1;
			return 0;
		}

		// increase hops to check for timeout to re-first-analyze
		if ( ++analyzer->trackAnalyzeTimes >= ANALYZER_TRACK_ANALYZE_TIMEOUT ) {
			analyzer->firstAnalyzeTimes = -1;
			analyzer->trackAnalyzeTimes = ANALYZER_TRACK_ANALYZE_TIMEOUT;
		}
	}

	return 0;
}

void sb::release( sb::Analyzer* analyzer ) {}

int sb::firstAnalyze( sb::Analyzer* analyzer,
											sb::FrameInfo* frameInfo,
											sb::RoadInfo* roadInfo )
{
	sb::Timer timer;
	timer.reset( "find" );
	if ( analyzer->leftLane.find( frameInfo ) < 0 ) return -1;
	if ( analyzer->rightLane.find( frameInfo ) < 0 ) return -1;
	std::cout << "find: " << timer.milliseconds( "find" ) << "ms." << std::endl;

	cv::Mat colorImage = frameInfo->colorImage.clone();

	auto it_left_part = analyzer->leftLane.getParts().cbegin();
	auto it_right_part = analyzer->rightLane.getParts().cbegin();
	for ( ; it_left_part != analyzer->leftLane.getParts().cend(); ++it_left_part , ++it_right_part ) {
		cv::line( colorImage,
		          it_left_part->part.innerLine.getBottomPoint(), it_left_part->part.innerLine.getTopPoint(), cv::Scalar( 0, 255, 0 ), 2 );
		cv::line( colorImage,
		          it_left_part->part.outerLine.getBottomPoint(), it_left_part->part.outerLine.getTopPoint(), cv::Scalar( 0, 255, 0 ), 2 );
		cv::line( colorImage,
		          it_right_part->part.innerLine.getBottomPoint(), it_right_part->part.innerLine.getTopPoint(), cv::Scalar( 0, 255, 0 ), 2 );
		cv::line( colorImage,
		          it_right_part->part.outerLine.getBottomPoint(), it_right_part->part.outerLine.getTopPoint(), cv::Scalar( 0, 255, 0 ), 2 );
	}

	cv::imshow( "Analyzer", colorImage );
	cv::waitKey();

	return 0;
}

int sb::trackAnalyze( sb::Analyzer* analyzer,
											sb::FrameInfo* frameInfo,
											sb::RoadInfo* roadInfo )
{
	analyzer->leftLane.track( frameInfo );
	analyzer->rightLane.track( frameInfo );

	cv::Mat colorImage = frameInfo->colorImage.clone();

	auto it_left_part = analyzer->leftLane.getParts().cbegin();
	auto it_right_part = analyzer->rightLane.getParts().cbegin();
	for ( ; it_left_part != analyzer->leftLane.getParts().cend(); ++it_left_part , ++it_right_part ) {
		cv::line( colorImage,
		          it_left_part->part.innerLine.getBottomPoint(), it_left_part->part.innerLine.getTopPoint(), cv::Scalar( 0, 255, 0 ), 2 );
		cv::line( colorImage,
		          it_left_part->part.outerLine.getBottomPoint(), it_left_part->part.outerLine.getTopPoint(), cv::Scalar( 0, 255, 0 ), 2 );
		cv::line( colorImage,
		          it_right_part->part.innerLine.getBottomPoint(), it_right_part->part.innerLine.getTopPoint(), cv::Scalar( 0, 255, 0 ), 2 );
		cv::line( colorImage,
		          it_right_part->part.outerLine.getBottomPoint(), it_right_part->part.outerLine.getTopPoint(), cv::Scalar( 0, 255, 0 ), 2 );
	}

	cv::imshow( "Analyzer", colorImage );
	cv::waitKey();

	/*
	 * _leftLane.track(frameInfo);
	 * _rightLane.track(frameInfo);
	 *
	 *if(_leftLane.getError() == LOST_LANE && _rightLane.getError() == LOST_LANE) return -1;
	 *
	 *if(_leftLane.getError() == LOST_LANE) {
	 *	findLeftLaneWithRightLane();
	 *}
	 *if(_rightLane.getError() == LOST_LANE) {
	 *	findRightLaneWithLeftLane();
	 *}
	 *
	 *target = (_leftLane.getTop() + _rightLane.getTop())*0.5
	 */

	return 0;
}
