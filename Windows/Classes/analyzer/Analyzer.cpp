#include "Analyzer.h"

int sb::init( sb::Analyzer* analyzer, sb::Params* params )
{
	analyzer->firstAnalyzeTimes = -1;

	analyzer->trackAnalyzeTimes = ANALYZER_TRACK_ANALYZE_TIMEOUT;

	analyzer->leftLane = new sb::LaneComponent;
	sb::init( analyzer->leftLane, -1 );

	analyzer->rightLane = new sb::LaneComponent;
	sb::init( analyzer->rightLane, 1 );

	return 0;
}

int sb::analyze( sb::Analyzer* analyzer,
                 sb::FrameInfo* frameInfo,
                 sb::RoadInfo* roadInfo )
{
	// first analyze
	if ( analyzer->firstAnalyzeTimes < ANALYZER_FIRST_ANALYZE_TIMEOUT ) {
		if ( firstAnalyze( analyzer, frameInfo, roadInfo ) >= 0 ) {
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
		if ( trackAnalyze( analyzer, frameInfo, roadInfo ) >= 0 ) {
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

void sb::release( sb::Analyzer* analyzer )
{
	sb::release( analyzer->leftLane );
	delete analyzer->leftLane;

	sb::release( analyzer->rightLane );
	delete analyzer->rightLane;
}

int sb::firstAnalyze( sb::Analyzer* analyzer,
                      sb::FrameInfo* frameInfo,
                      sb::RoadInfo* roadInfo )
{
	if ( sb::find( analyzer->leftLane, frameInfo ) < 0 ) return -1;
	if ( sb::find( analyzer->rightLane, frameInfo ) < 0 ) return -1;

	cv::Point target( (analyzer->leftLane->parts.back()->part->origin.x + analyzer->rightLane->parts.back()->part->origin.x) / 2,
	                  (analyzer->leftLane->parts.back()->part->origin.y + analyzer->rightLane->parts.back()->part->origin.y) / 2 );

	// debug
	{
		cv::Mat img = frameInfo->bgrImage.clone();
		size_t n_parts = analyzer->leftLane->parts.size();

		for ( size_t i = 1; i < n_parts; ++i ) {
			cv::line( img,
			          analyzer->leftLane->parts[i]->part->origin,
			          analyzer->leftLane->parts[i - 1]->part->origin,
			          cv::Scalar( 0, 255, 0 ), 3 );
			cv::line( img,
			          analyzer->rightLane->parts[i]->part->origin,
			          analyzer->rightLane->parts[i - 1]->part->origin,
			          cv::Scalar( 0, 255, 0 ), 3 );
		}
		cv::imshow( "Lanes detected", img );
		cv::waitKey();
	}

	return 0;
}

int sb::trackAnalyze( sb::Analyzer* analyzer,
                      sb::FrameInfo* frameInfo,
                      sb::RoadInfo* roadInfo )
{
	sb::track( analyzer->leftLane, frameInfo );
	sb::track( analyzer->rightLane, frameInfo );

	// debug
	{
		cv::Mat img = frameInfo->bgrImage.clone();

		auto it_part_left = analyzer->leftLane->parts.cbegin();
		auto it_part_right = analyzer->rightLane->parts.cbegin();
		for ( ; it_part_left != analyzer->leftLane->parts.cend(); ++it_part_left , ++it_part_right ) {
			sb::LanePartInfo* leftPart = *it_part_left;
			sb::LanePartInfo* rightPart = *it_part_right;

			cv::Scalar color;

			if( leftPart->errorCode == sb::PART_NICE ) color = cv::Scalar( 0, 255, 0 );
			else if( leftPart->errorCode == sb::PART_OUTSIGHT_LEFT || leftPart->errorCode == sb::PART_OUTSIGHT_RIGHT ) color = cv::Scalar( 0, 255, 255 );
			else color = cv::Scalar( 0, 0, 255 );
			cv::circle( img, leftPart->part->origin, 5, color, 2 );

			if( rightPart->errorCode == sb::PART_NICE ) color = cv::Scalar( 0, 255, 0 );
			else if( rightPart->errorCode == sb::PART_OUTSIGHT_LEFT || rightPart->errorCode == sb::PART_OUTSIGHT_RIGHT ) color = cv::Scalar( 0, 255, 255 );
			else color = cv::Scalar( 0, 0, 255 );
			cv::circle( img, rightPart->part->origin, 5, color, 2 );
		}

		cv::imshow( "Lanes detected", img );
		cv::waitKey();
	}

	return 0;
}
