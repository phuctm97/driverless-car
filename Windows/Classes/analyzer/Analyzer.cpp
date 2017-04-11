#include "Analyzer.h"

int sb::Analyzer::init( const sb::Params& params )
{
	_firstAnalyzeTimes = -1;

	_trackAnalyzeTimes = ANALYZER_TRACK_ANALYZE_TIMEOUT;

	_leftLane.init( -1 );

	_rightLane.init( 1 );

	return 0;
}

int sb::Analyzer::analyze( const sb::FrameInfo& frameInfo, sb::RoadInfo& roadInfo )
{
	// first analyze
	if ( _firstAnalyzeTimes < ANALYZER_FIRST_ANALYZE_TIMEOUT ) {
		if ( firstAnalyze( frameInfo, roadInfo ) >= 0 ) {
			// success, stop first analyze, go to analyze track
			_firstAnalyzeTimes = ANALYZER_FIRST_ANALYZE_TIMEOUT;
			_trackAnalyzeTimes = -1;
			return 0;
		}

		// increase hops and check for timeout to stop system
		if ( ++_firstAnalyzeTimes >= ANALYZER_FIRST_ANALYZE_TIMEOUT ) return -1;
	}

	// track analyze
	if ( _trackAnalyzeTimes < ANALYZER_TRACK_ANALYZE_TIMEOUT ) {
		if ( trackAnalyze( frameInfo, roadInfo ) >= 0 ) {
			// success, to next frame
			_trackAnalyzeTimes = -1;
			return 0;
		}

		// increase hops to check for timeout to re-first-analyze
		if ( ++_trackAnalyzeTimes >= ANALYZER_TRACK_ANALYZE_TIMEOUT ) {
			_firstAnalyzeTimes = -1;
			_trackAnalyzeTimes = ANALYZER_TRACK_ANALYZE_TIMEOUT;
		}
	}

	return 0;
}

void sb::Analyzer::release() {}

int sb::Analyzer::firstAnalyze( const sb::FrameInfo& frameInfo,
                                sb::RoadInfo& roadInfo )
{
	sb::Timer timer;
	timer.reset( "find" );
	if ( _leftLane.find( frameInfo ) < 0 ) return -1;
	if ( _rightLane.find( frameInfo ) < 0 ) return -1;
	std::cout << "find: " << timer.milliseconds( "find" ) << "ms." << std::endl;

	cv::Mat colorImage = frameInfo.getColorImage();

	auto it_left_part = _leftLane.getParts().cbegin();
	auto it_right_part = _rightLane.getParts().cbegin();
	for ( ; it_left_part != _leftLane.getParts().cend(); ++it_left_part , ++it_right_part ) {
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

int sb::Analyzer::trackAnalyze( const sb::FrameInfo& frameInfo, sb::RoadInfo& roadInfo )
{
	_leftLane.track( frameInfo );
	_rightLane.track( frameInfo );

	cv::Mat colorImage = frameInfo.getColorImage();

	auto it_left_part = _leftLane.getParts().cbegin();
	auto it_right_part = _rightLane.getParts().cbegin();
	for ( ; it_left_part != _leftLane.getParts().cend(); ++it_left_part , ++it_right_part ) {
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
