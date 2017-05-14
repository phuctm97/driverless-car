#include "Analyzer.h"

int sb::init( sb::Analyzer* analyzer, sb::Params* params )
{
	analyzer->firstAnalyzeTimes = -1;

	analyzer->trackAnalyzeTimes = ANALYZER_TRACK_ANALYZE_TIMEOUT;

	analyzer->repo = new sb::Repository;

	analyzer->knots.clear();

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
	analyzer->firstAnalyzeTimes = -1;
	analyzer->trackAnalyzeTimes = ANALYZER_TRACK_ANALYZE_TIMEOUT;

	sb::release( analyzer->repo );
	delete analyzer->repo;
}

int sb::firstAnalyze( sb::Analyzer* analyzer,
                      sb::FrameInfo* frameInfo,
                      sb::RoadInfo* roadInfo )
{
	findLanes( analyzer, frameInfo );

	analyzeResult( analyzer, frameInfo, roadInfo );

	if ( analyzer->roadState == sb::RoadState::UNKNOWN ) return -1;

	return 0;
}

int sb::trackAnalyze( sb::Analyzer* analyzer,
                      sb::FrameInfo* frameInfo,
                      sb::RoadInfo* roadInfo )
{
	trackLanes( analyzer, frameInfo );

	analyzeResult( analyzer, frameInfo, roadInfo );

	if ( analyzer->roadState == sb::RoadState::UNKNOWN ) return -1;

	return 0;
}

void sb::findLanes( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo )
{
	findBothLanes( analyzer, frameInfo );

	// release other blobs
	for ( auto it_blob = frameInfo->blobs.begin(); it_blob != frameInfo->blobs.end(); ++it_blob ) {
		sb::Blob* blob = *it_blob;
		if ( blob == analyzer->repo->leftBlob || blob == analyzer->repo->rightBlob ) continue;

		sb::release( blob );
		delete blob;
		*it_blob = nullptr;
	}
}

void sb::findBothLanes( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo )
{
	if ( frameInfo->blobs.empty() ) {
		analyzer->roadState = sb::RoadState::UNKNOWN;
		return;
	}

<<<<<<< HEAD
	// allocate road width repo
	if ( analyzer->repo->roadWidths.empty() ) {
		analyzer->repo->roadWidths.assign( frameInfo->blobs.front()->childBlobs.size(), 0 );
	}
=======
	/*// create real image
	const int W = 900;
	const int H = 700;
	cv::Mat realImage( frameInfo.getColorImage().rows + H,
	                   frameInfo.getColorImage().cols + W, CV_8UC3,
	                   cv::Scalar( 0, 0, 0 ) );
>>>>>>> master

	// two largest blob, to left most is left lane, the right most is right lane
	Blob* largestBlobs[2] = { nullptr };
	for ( auto cit_blob = frameInfo->blobs.cbegin(); cit_blob != frameInfo->blobs.cend(); ++cit_blob ) {
		sb::Blob* blob = *cit_blob;

		if ( largestBlobs[0] == nullptr || blob->size > largestBlobs[0]->size ) {
			largestBlobs[1] = largestBlobs[0];
			largestBlobs[0] = blob;
		}
		else if ( largestBlobs[1] == nullptr || blob->size > largestBlobs[1]->size ) {
			largestBlobs[1] = blob;
		}
	}

	// both line must be obtained, and in good condition
	if ( largestBlobs[0] == nullptr || largestBlobs[1] == nullptr
		|| largestBlobs[0]->size < MIN_ACCEPTABLE_FULL_LANE_BLOB_OBJECTS_COUNT
		|| largestBlobs[1]->size < MIN_ACCEPTABLE_FULL_LANE_BLOB_OBJECTS_COUNT
		|| std::count_if( largestBlobs[0]->childBlobs.cbegin(), largestBlobs[0]->childBlobs.cend(),
		                  []( sb::Blob* b ) { return b->size > MIN_ACCEPTABLE_CHILD_BLOB_OBJECTS_COUNT; } ) < 3
		|| std::count_if( largestBlobs[1]->childBlobs.cbegin(), largestBlobs[1]->childBlobs.cend(),
		                  []( sb::Blob* b ) { return b->size > MIN_ACCEPTABLE_CHILD_BLOB_OBJECTS_COUNT; } ) < 3 ) {
		analyzer->roadState = sb::RoadState::UNKNOWN;
		return;
	}

	// largestBlobs[0] is left, largestBlobs[1] is right
	if ( largestBlobs[1]->origin.x < largestBlobs[0]->origin.x ) {
		auto tmp = largestBlobs[1];
		largestBlobs[1] = largestBlobs[0];
		largestBlobs[0] = tmp;
	}

<<<<<<< HEAD
	sb::release( analyzer->repo );
	analyzer->repo->leftBlob = largestBlobs[0];
	analyzer->repo->rightBlob = largestBlobs[1];
	analyzer->roadState = sb::RoadState::BOTH_LANE_DETECTED;
}
=======
	cv::imshow( "Test Analyzer", realImage );
	cv::waitKey();*/
>>>>>>> master

void sb::trackLanes( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo )
{
	bool trackSucceeded = false;

	for ( auto cit_next_state = analyzer->repo->possibleNextStates.cbegin(); cit_next_state != analyzer->repo->possibleNextStates.cend(); ++cit_next_state ) {
		switch ( *cit_next_state ) {
		case sb::RoadState::BOTH_LANE_DETECTED: {
			trackBothLanes( analyzer, frameInfo );
			if ( analyzer->roadState == sb::RoadState::BOTH_LANE_DETECTED ) trackSucceeded = true;
		}
			break;

		case sb::RoadState::IGNORE_LEFT_LANE: {
			trackRightLane( analyzer, frameInfo );
			if ( analyzer->roadState == sb::RoadState::IGNORE_LEFT_LANE ) trackSucceeded = true;
		}
			break;

		case sb::RoadState::IGNORE_RIGHT_LANE: {
			trackLeftLane( analyzer, frameInfo );
			if ( analyzer->roadState == sb::RoadState::IGNORE_RIGHT_LANE ) trackSucceeded = true;
		}
			break;
		}

		if ( trackSucceeded ) break;
	}

	// release other blobs
	for ( auto it_blob = frameInfo->blobs.begin(); it_blob != frameInfo->blobs.end(); ++it_blob ) {
		sb::Blob* blob = *it_blob;
		if ( blob == analyzer->repo->leftBlob || blob == analyzer->repo->rightBlob ) continue;

		sb::release( blob );
		delete blob;
		*it_blob = nullptr;
	}
}

void sb::trackBothLanes( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo )
{
	// two largest blob, to left most is left lane, the right most is right lane
	Blob* largestBlobs[2] = { nullptr };
	for ( auto cit_blob = frameInfo->blobs.cbegin(); cit_blob != frameInfo->blobs.cend(); ++cit_blob ) {
		sb::Blob* blob = *cit_blob;

		if ( largestBlobs[0] == nullptr || blob->size > largestBlobs[0]->size ) {
			largestBlobs[1] = largestBlobs[0];
			largestBlobs[0] = blob;
		}
		else if ( largestBlobs[1] == nullptr || blob->size > largestBlobs[1]->size ) {
			largestBlobs[1] = blob;
		}
	}

	// both line must be obtained, and in good condition
	if ( largestBlobs[0] == nullptr || largestBlobs[1] == nullptr
		|| largestBlobs[0]->size < MIN_ACCEPTABLE_FULL_LANE_BLOB_OBJECTS_COUNT
		|| largestBlobs[1]->size < MIN_ACCEPTABLE_FULL_LANE_BLOB_OBJECTS_COUNT
		|| std::count_if( largestBlobs[0]->childBlobs.cbegin(), largestBlobs[0]->childBlobs.cend(),
		                  []( sb::Blob* blob ) { return blob->size > MIN_ACCEPTABLE_CHILD_BLOB_OBJECTS_COUNT; } ) < 3
		|| std::count_if( largestBlobs[1]->childBlobs.cbegin(), largestBlobs[1]->childBlobs.cend(),
		                  []( sb::Blob* blob ) { return blob->size > MIN_ACCEPTABLE_CHILD_BLOB_OBJECTS_COUNT; } ) < 3 ) {
		analyzer->roadState = sb::RoadState::UNKNOWN;
		return;
	}

	// largestBlobs[0] is left, largestBlobs[1] is right
	if ( largestBlobs[1]->origin.x < largestBlobs[0]->origin.x ) {
		auto tmp = largestBlobs[1];
		largestBlobs[1] = largestBlobs[0];
		largestBlobs[0] = tmp;
	}

	sb::release( analyzer->repo );
	analyzer->repo->leftBlob = largestBlobs[0];
	analyzer->repo->rightBlob = largestBlobs[1];
	analyzer->roadState = sb::RoadState::BOTH_LANE_DETECTED;
}

void sb::trackLeftLane( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo )
{
	sb::Blob* largestBlob = nullptr;
	for ( auto cit_blob = frameInfo->blobs.cbegin(); cit_blob != frameInfo->blobs.cend(); ++cit_blob ) {
		sb::Blob* blob = *cit_blob;

		if ( abs( blob->origin.x - analyzer->repo->leftBlob->origin.x ) > MAX_ACCEPTABLE_LANE_POSITION_DIFF ) continue;

		if ( largestBlob == nullptr || blob->size > largestBlob->size ) {
			largestBlob = blob;
		}
	}

	if ( largestBlob == nullptr
		|| largestBlob->size < MIN_ACCEPTABLE_FULL_LANE_BLOB_OBJECTS_COUNT
		|| std::count_if( largestBlob->childBlobs.cbegin(), largestBlob->childBlobs.cend(),
		                  []( sb::Blob* blob ) { return blob->size > MIN_ACCEPTABLE_CHILD_BLOB_OBJECTS_COUNT; } ) < 3 ) {
		analyzer->roadState = sb::RoadState::UNKNOWN;
		return;
	}

	sb::release( analyzer->repo );
	analyzer->repo->leftBlob = largestBlob;
	analyzer->roadState = sb::RoadState::IGNORE_RIGHT_LANE;
}

void sb::trackRightLane( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo )
{
	sb::Blob* largestBlob = nullptr;
	for ( auto cit_blob = frameInfo->blobs.cbegin(); cit_blob != frameInfo->blobs.cend(); ++cit_blob ) {
		sb::Blob* blob = *cit_blob;

		if ( abs( blob->origin.x - analyzer->repo->rightBlob->origin.x ) > MAX_ACCEPTABLE_LANE_POSITION_DIFF ) continue;

		if ( largestBlob == nullptr || blob->size > largestBlob->size ) {
			largestBlob = blob;
		}
	}

	if ( largestBlob == nullptr
		|| largestBlob->size < MIN_ACCEPTABLE_FULL_LANE_BLOB_OBJECTS_COUNT
		|| std::count_if( largestBlob->childBlobs.cbegin(), largestBlob->childBlobs.cend(),
		                  []( sb::Blob* blob ) { return blob->size > MIN_ACCEPTABLE_CHILD_BLOB_OBJECTS_COUNT; } ) < 3 ) {
		analyzer->roadState = sb::RoadState::UNKNOWN;
		return;
	}

	sb::release( analyzer->repo );
	analyzer->repo->rightBlob = largestBlob;
	analyzer->roadState = sb::RoadState::IGNORE_LEFT_LANE;
}

void sb::analyzeWithBothLane( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo, sb::RoadInfo* roadInfo )
{
	analyzer->repo->possibleNextStates.clear();
	analyzer->knots.reserve( analyzer->repo->leftBlob->childBlobs.size() );

	auto cit_left_blob = analyzer->repo->leftBlob->childBlobs.cbegin(); // left child blobs
	auto cit_right_blob = analyzer->repo->rightBlob->childBlobs.cbegin(); // right child blobs
	auto it_road_width = analyzer->repo->roadWidths.begin(); // for update road width repo

	int voteIgnoreLeft = 0;
	int voteIgnoreRight = 0;

	for ( ; cit_left_blob != analyzer->repo->leftBlob->childBlobs.cend(); ++cit_left_blob , ++cit_right_blob , ++it_road_width ) {
		sb::Blob* leftBlob = *cit_left_blob;
		sb::Blob* rightBlob = *cit_right_blob;

		sb::LaneKnot first;
		if ( leftBlob->size > MIN_ACCEPTABLE_CHILD_BLOB_OBJECTS_COUNT ) {
			first.position = leftBlob->origin;
			first.type = 1;
		}
		else { // error left child blob
			first.type = -1;
		}

		sb::LaneKnot second;
		if ( rightBlob->size > MIN_ACCEPTABLE_CHILD_BLOB_OBJECTS_COUNT ) {
			second.position = rightBlob->origin;
			second.type = 1;
		}
		else { // error right child blob
			second.type = -1;
		}

		analyzer->knots.push_back( std::make_pair( first, second ) );

		if ( first.type > 0 && second.type > 0 ) { // good pair of lane
			roadInfo->target = (first.position + second.position) * 0.5; // calculate road target

			// update road width repo
			if ( *it_road_width == 0 || (leftBlob->box.tl().x > 1 && rightBlob->box.br().x < frameInfo->bgrImage.cols - 2) ) {
				*it_road_width = second.position.x - first.position.x;
			}
		}

		if ( first.type <= 0 || leftBlob->box.tl().x < 5 ) voteIgnoreLeft++;
		if ( second.type <= 0 || rightBlob->box.br().x >= frameInfo->bgrImage.cols - 5 ) voteIgnoreRight++;
	}

	analyzer->repo->possibleNextStates.push_back( sb::RoadState::BOTH_LANE_DETECTED );
	if ( voteIgnoreLeft >= 2 ) analyzer->repo->possibleNextStates.push_back( sb::RoadState::IGNORE_LEFT_LANE );
	if ( voteIgnoreRight >= 2 ) analyzer->repo->possibleNextStates.push_back( sb::RoadState::IGNORE_RIGHT_LANE );
}

void sb::analyzeWithoutLeftLane( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo, sb::RoadInfo* roadInfo )
{
	analyzer->repo->possibleNextStates.clear();
	analyzer->knots.reserve( analyzer->repo->rightBlob->childBlobs.size() );

	auto cit_right_blob = analyzer->repo->rightBlob->childBlobs.cbegin(); // road child blobs
	auto cit_road_width = analyzer->repo->roadWidths.cbegin(); // for estimate road center

	int voteForBothLane = 0;

	for ( ; cit_right_blob != analyzer->repo->rightBlob->childBlobs.cend(); ++cit_right_blob , ++cit_road_width ) {
		sb::Blob* rightBlob = *cit_right_blob;

		sb::LaneKnot first;
		first.type = -1;

		sb::LaneKnot second;
		if ( rightBlob->size > MIN_ACCEPTABLE_CHILD_BLOB_OBJECTS_COUNT ) {
			second.position = rightBlob->origin;
			second.type = 1;
		}
		else { // error right lane
			second.type = -1;
		}

		analyzer->knots.push_back( std::make_pair( first, second ) );

		// calculate road center
		if ( second.type > 0 ) {
			roadInfo->target = second.position - cv::Point( (*cit_road_width) / 2, 0 );

			if ( second.position.x - *cit_road_width > 0 ) voteForBothLane++;
		}
	}

	if ( voteForBothLane >= 2 ) analyzer->repo->possibleNextStates.push_back( sb::RoadState::BOTH_LANE_DETECTED );
	analyzer->repo->possibleNextStates.push_back( sb::RoadState::IGNORE_LEFT_LANE );
}

void sb::analyzeWithoutRightLane( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo, sb::RoadInfo* roadInfo )
{
	analyzer->repo->possibleNextStates.clear();
	analyzer->knots.reserve( analyzer->repo->leftBlob->childBlobs.size() );

	auto cit_left_blob = analyzer->repo->leftBlob->childBlobs.cbegin(); // road child blobs
	auto cit_road_width = analyzer->repo->roadWidths.cbegin(); // for estimate road center

	int voteForBothLane = 0;

	for ( ; cit_left_blob != analyzer->repo->leftBlob->childBlobs.cend(); ++cit_left_blob , ++cit_road_width ) {
		sb::Blob* leftBlob = *cit_left_blob;

		sb::LaneKnot first;
		if ( leftBlob->size > MIN_ACCEPTABLE_CHILD_BLOB_OBJECTS_COUNT ) {
			first.position = leftBlob->origin;
			first.type = 1;
		}
		else {
			first.type = -1;
		}

		sb::LaneKnot second;
		second.type = -1;

		analyzer->knots.push_back( std::make_pair( first, second ) );

		// calculate road center
		if ( first.type > 0 ) {
			roadInfo->target = first.position + cv::Point( (*cit_road_width) / 2, 0 );

			if ( first.position.x + *cit_road_width < frameInfo->bgrImage.cols ) voteForBothLane++;
		}
	}

	if ( voteForBothLane >= 2 ) analyzer->repo->possibleNextStates.push_back( sb::RoadState::BOTH_LANE_DETECTED );
	analyzer->repo->possibleNextStates.push_back( sb::RoadState::IGNORE_RIGHT_LANE );
}

<<<<<<< HEAD
void sb::analyzeResult( sb::Analyzer* analyzer, sb::FrameInfo* frameInfo, sb::RoadInfo* roadInfo )
{
	analyzer->knots.clear();

	switch ( analyzer->roadState ) {

	case sb::RoadState::BOTH_LANE_DETECTED: {
		sb::analyzeWithBothLane( analyzer, frameInfo, roadInfo );
	}
		break;

	case sb::RoadState::IGNORE_LEFT_LANE: {
		analyzeWithoutLeftLane( analyzer, frameInfo, roadInfo );
	}
		break;

	case sb::RoadState::IGNORE_RIGHT_LANE: {
		analyzeWithoutRightLane( analyzer, frameInfo, roadInfo );
	}
		break;

	case sb::RoadState::UNKNOWN: {
		roadInfo->target = cv::Point( 0, 0 );
	}
		break;
	}
}
=======
void sb::Analyzer::release() { }

>>>>>>> master
