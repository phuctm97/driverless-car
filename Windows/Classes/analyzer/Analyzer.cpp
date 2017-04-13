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
	analyzer->roadWidths.clear();
	analyzer->firstAnalyzeTimes = -1;
	analyzer->trackAnalyzeTimes = ANALYZER_TRACK_ANALYZE_TIMEOUT;

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

	// calculate road width
	{
		analyzer->roadWidths.clear();
		analyzer->roadWidths.assign( frameInfo->imageSections.size(), 0 );
		auto it_road_width = analyzer->roadWidths.begin();
		auto cit_left_part = analyzer->leftLane->parts.cbegin();
		auto cit_right_part = analyzer->rightLane->parts.cbegin();
		for ( ; it_road_width != analyzer->roadWidths.end(); ++it_road_width , ++cit_left_part , ++cit_right_part ) {
			*it_road_width = (*cit_right_part)->part->origin.x - (*cit_left_part)->part->origin.x;
		}
	}

	// calculate target
	roadInfo->target = cv::Point( (analyzer->leftLane->parts[2]->part->origin.x + analyzer->rightLane->parts[2]->part->origin.x) / 2,
	                              (analyzer->leftLane->parts[2]->part->origin.y + analyzer->rightLane->parts[2]->part->origin.y) / 2 );

#ifdef SB_DEBUG_ANALYZER
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
		cv::circle( img, roadInfo->target, 5, cv::Scalar( 255, 255, 255 ), -1 );
		cv::circle( img, roadInfo->target, 5, cv::Scalar( 0, 0, 0 ), 3 ); 
		cv::imshow( "Lanes detected", img );
		cv::waitKey();
	}
#endif

	return 0;
}

int sb::trackAnalyze( sb::Analyzer* analyzer,
                      sb::FrameInfo* frameInfo,
                      sb::RoadInfo* roadInfo )
{
	if ( sb::track( analyzer->leftLane, frameInfo ) < 0 ) return -1;
	if ( sb::track( analyzer->rightLane, frameInfo ) < 0 )return -1;

	// update road width
	{
		auto it_road_width = analyzer->roadWidths.begin();
		auto cit_left_part = analyzer->leftLane->parts.cbegin();
		auto cit_right_part = analyzer->rightLane->parts.cbegin();
		for ( ; it_road_width != analyzer->roadWidths.end(); ++it_road_width , ++cit_left_part , ++cit_right_part ) {
			sb::LanePartInfo* leftPart = *cit_left_part;
			sb::LanePartInfo* rightPart = *cit_right_part;
			if ( leftPart->errorCode != sb::PART_NICE || rightPart->errorCode != sb::PART_NICE ) continue;

			*it_road_width = rightPart->part->origin.x - leftPart->part->origin.x;
		}
	}

	// fix error part
	{
		auto cit_road_width = analyzer->roadWidths.cbegin();
		auto cit_left_part = analyzer->leftLane->parts.cbegin();
		auto cit_right_part = analyzer->rightLane->parts.cbegin();
		for ( ; cit_road_width != analyzer->roadWidths.cend(); ++cit_road_width , ++cit_left_part , ++cit_right_part ) {
			sb::LanePartInfo* leftPart = *cit_left_part;
			sb::LanePartInfo* rightPart = *cit_right_part;

			if ( leftPart->errorCode == sb::PART_NICE && rightPart->errorCode == sb::PART_NICE ) continue;
			if ( leftPart->errorCode != sb::PART_NICE && rightPart->errorCode != sb::PART_NICE ) continue;

			if ( leftPart->errorCode != sb::PART_NICE )
				leftPart->part->origin.x = rightPart->part->origin.x - *cit_road_width;
			else
				rightPart->part->origin.x = leftPart->part->origin.x + *cit_road_width;
		}
	}

	// check for all parts failed, return -1 (stop)
	{
		bool trackFailed = true;

		auto cit_left_part = analyzer->leftLane->parts.cbegin();
		auto cit_right_part = analyzer->rightLane->parts.cbegin();
		for ( ; cit_left_part != analyzer->leftLane->parts.cend(); ++cit_left_part , ++cit_right_part ) {
			if ( (*cit_left_part)->errorCode == sb::PART_NICE || (*cit_right_part)->errorCode == sb::PART_NICE ) {
				trackFailed = false;
				break;
			}
		}

		if ( trackFailed ) return -1;
	}

	// calculate target
	roadInfo->target = cv::Point( (analyzer->leftLane->parts[2]->part->origin.x + analyzer->rightLane->parts[2]->part->origin.x) / 2,
	                              (analyzer->leftLane->parts[2]->part->origin.y + analyzer->rightLane->parts[2]->part->origin.y) / 2 );

#ifdef SB_DEBUG_ANALYZER
	{
		cv::Mat img = frameInfo->bgrImage.clone();

		auto it_part_left = analyzer->leftLane->parts.cbegin();
		auto it_part_right = analyzer->rightLane->parts.cbegin();
		for ( ; it_part_left != analyzer->leftLane->parts.cend(); ++it_part_left , ++it_part_right ) {
			sb::LanePartInfo* leftPart = *it_part_left;
			sb::LanePartInfo* rightPart = *it_part_right;

			cv::Scalar color;

			if ( leftPart->errorCode == sb::PART_NICE ) color = cv::Scalar( 0, 255, 0 );
			else if ( leftPart->errorCode == sb::PART_OUTSIGHT_LEFT || leftPart->errorCode == sb::PART_OUTSIGHT_RIGHT ) color = cv::Scalar( 0, 255, 255 );
			else color = cv::Scalar( 0, 0, 255 );
			cv::circle( img, leftPart->part->origin, 5, color, 2 );

			if ( rightPart->errorCode == sb::PART_NICE ) color = cv::Scalar( 0, 255, 0 );
			else if ( rightPart->errorCode == sb::PART_OUTSIGHT_LEFT || rightPart->errorCode == sb::PART_OUTSIGHT_RIGHT ) color = cv::Scalar( 0, 255, 255 );
			else color = cv::Scalar( 0, 0, 255 );
			cv::circle( img, rightPart->part->origin, 5, color, 2 );
		}
		cv::circle( img, roadInfo->target, 5, cv::Scalar( 255, 255, 255 ), -1 );
		cv::circle( img, roadInfo->target, 5, cv::Scalar( 0, 0, 0 ), 3 );

		cv::imshow( "Lanes detected", img );
		cv::waitKey();
	}
#endif

	return 0;
}
