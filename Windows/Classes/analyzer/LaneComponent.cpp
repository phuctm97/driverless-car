#include "LaneComponent.h"

// TODO: add more compicated relative value between lane part to rate more accurate
// TODO: analyze inner color and outer color of lane

void sb::release( sb::LaneComponent* laneComponent )
{
	for ( auto part : laneComponent->parts ) {
		sb::release( part );
		delete part;
	}
	laneComponent->parts.clear();
}

void sb::init( sb::LaneComponent* laneComponent, int side, int minLaneWidth, int maxLaneWidth )
{
	laneComponent->side = side;

	laneComponent->minLaneWidth = minLaneWidth;

	laneComponent->maxLaneWidth = maxLaneWidth;
}

int sb::find( sb::LaneComponent* laneComponent, sb::FrameInfo* frameInfo )
{
	if ( frameInfo->imageSections.size() < 3 ) {
		std::cerr << "Frame must be splitted into at least 3 sections." << std::endl;
		return -1;
	}

	std::queue<sb::LanePart*> poolParts; // for release memory

	std::vector<sb::LanePart*> bestLaneParts; // final result
	float highestRating = 0;

	// find first lane parts
	std::vector<sb::LanePart*> firstLaneParts;
	findFirstLaneParts( laneComponent, frameInfo, firstLaneParts );

	// push new parts to pool
	for ( auto cit_first_lane_part = firstLaneParts.cbegin(); cit_first_lane_part != firstLaneParts.cend(); ++cit_first_lane_part )
		poolParts.push( *cit_first_lane_part );

	// find full sequence of lane parts from these first lane parts
	for ( auto cit_first_lane_part = firstLaneParts.cbegin(); cit_first_lane_part != firstLaneParts.cend(); ++cit_first_lane_part ) {
		std::vector<sb::LanePart*> fullLaneParts;
		float fullLaneRating;

		sb::findBestLaneParts( laneComponent, frameInfo, *cit_first_lane_part, fullLaneParts, fullLaneRating );

		if ( fullLaneRating > highestRating ) {
			bestLaneParts = fullLaneParts;
			highestRating = fullLaneRating;
		}

		for ( auto cit_full_lane_part = fullLaneParts.cbegin(); cit_full_lane_part != fullLaneParts.cend(); ++cit_full_lane_part )
			poolParts.push( *cit_full_lane_part ); // push new parts to pool

	}

	// good sequence of lane parts detected, copy it to component's properties
	if ( highestRating > 3.0f ) {

		// clear old parts
		for ( auto cit_part = laneComponent->parts.cbegin(); cit_part != laneComponent->parts.cend(); ++cit_part ) {
			sb::release( *cit_part );
			delete *cit_part;
		}
		laneComponent->parts.clear();
		laneComponent->parts.reserve( 5 );

		// allocate new parts
		for ( auto cit_part = bestLaneParts.cbegin(); cit_part != bestLaneParts.cend(); ++cit_part ) {
			sb::LanePartInfo* partInfo = new sb::LanePartInfo;
			partInfo->part = new sb::LanePart( **cit_part );
			partInfo->errorCode = sb::PART_NICE;
			partInfo->errorAngle = 0;
			partInfo->errorColor = 0;
			partInfo->errorPosition = 0;
			partInfo->errorWidth = 0;

			laneComponent->parts.push_back( partInfo );
		}

	}

	// release pool
	while ( !poolParts.empty() ) {
		delete poolParts.front();
		poolParts.pop();
	}

	return highestRating > 3.0f ? 0 : -1;
}

void sb::findFirstLaneParts( sb::LaneComponent* laneComponent, sb::FrameInfo* frameInfo, std::vector<sb::LanePart*>& firstLaneParts )
{
	int centerX = frameInfo->bgrImage.cols / 2;

	// TODO: more complicated shape for lane part

	for ( auto cit_blob = frameInfo->imageSections.front()->blobs.cbegin();
	      cit_blob != frameInfo->imageSections.front()->blobs.cend(); ++cit_blob ) {
		sb::Blob* blob = *cit_blob;

#ifdef SB_DEBUG_LANE_COMPONENT_FIND
		{
			cv::Mat img = frameInfo->bgrImage.clone();
			cv::rectangle( img, blob->box.tl(), blob->box.br(), cv::Scalar( 0, 0, 255 ), 2 );
			cv::circle( img, blob->origin, 3, cv::Scalar( 0, 255, 0 ), 2 );
			cv::imshow( "Full lane", img );
			cv::waitKey();
		}
#endif

		// check pos
		if ( (laneComponent->side < 0 && blob->origin.x > centerX)
			|| (laneComponent->side > 0 && blob->origin.x < centerX) )
			continue;

		// check width
		if ( blob->box.width < laneComponent->minLaneWidth ) continue;

		// check color
		double colorError = sb::calculateDeltaE( blob->bgr, cv::Vec3b( 255, 255, 255 ) );
		if ( colorError > MAX_ACCEPTABLE_COLOR_ERROR_TO_WHITE ) continue;

		sb::LanePart* lanePart = new sb::LanePart;
		lanePart->origin = blob->origin;
		lanePart->box = blob->box;
		lanePart->bgr = blob->bgr;
		firstLaneParts.push_back( lanePart );
	}
}

void sb::findBestLaneParts( sb::LaneComponent* laneComponent, sb::FrameInfo* frameInfo,
                            sb::LanePart* firstLanePart,
                            std::vector<sb::LanePart*>& fullLaneParts, float& fullLaneRating )
{
	// reserve memory
	fullLaneRating = 0;
	fullLaneParts.reserve( frameInfo->imageSections.size() );

	std::queue<sb::LanePart*> poolParts; // for release parts

	std::stack<std::pair<std::vector<sb::LanePart*>, float>> stackParts; // for trav

	// init lane
	stackParts.push( std::make_pair( std::vector<sb::LanePart*>( 1, firstLanePart ), 0.0f ) );

	while ( !stackParts.empty() ) {

		std::vector<sb::LanePart*> parts = stackParts.top().first;
		float rating = stackParts.top().second;
		stackParts.pop();

#ifdef SB_DEBUG_LANE_COMPONENT_FIND
		{
			cv::Mat img = frameInfo->bgrImage.clone();
			for ( auto part : parts ) {
				cv::rectangle( img, part->box.tl(), part->box.br(), cv::Scalar( 0, 0, 255 ), 2 );
				cv::circle( img, part->origin, 3, cv::Scalar( 0, 255, 0 ), 2 );
			}
			cv::imshow( "Full lane", img );
			cv::waitKey();
		}
#endif

		// finish a lane
		if ( parts.size() == frameInfo->imageSections.size() ) {
			rating /= parts.size() - 1;

			if ( rating > fullLaneRating ) {
				fullLaneRating = rating;
				fullLaneParts = parts;
			}
			continue;
		}

		// find next part
		std::vector<std::pair<sb::LanePart*, float>> nextLaneParts;
		sb::findNextLaneParts( laneComponent, frameInfo, frameInfo->imageSections[parts.size()], parts.back(), nextLaneParts );

		// push to stack
		for ( auto cit_next_part = nextLaneParts.cbegin(); cit_next_part != nextLaneParts.cend(); ++cit_next_part ) {
			std::vector<sb::LanePart*> tempParts = parts;
			tempParts.push_back( cit_next_part->first );

			float tempRating = rating + cit_next_part->second;
			stackParts.push( std::make_pair( tempParts, tempRating ) );

			poolParts.push( cit_next_part->first ); // push new part to pool
		}

	}

	// move result to dedicated memory
	if ( fullLaneRating > 0 ) {
		for ( auto it_part = fullLaneParts.begin(); it_part != fullLaneParts.end(); ++it_part ) {
			*it_part = new sb::LanePart( **it_part );
		}
	}

	// release memory
	while ( !poolParts.empty() ) {
		delete poolParts.front();
		poolParts.pop();
	}
}

void sb::findNextLaneParts( sb::LaneComponent* laneComponent, sb::FrameInfo* frameInfo,
                            sb::Section* section, sb::LanePart* lastestLanePart,
                            std::vector<std::pair<sb::LanePart*, float>>& nextLaneParts )
{
	for ( auto cit_blob = section->blobs.cbegin(); cit_blob != section->blobs.cend(); ++cit_blob ) {

		sb::Blob* blob = *cit_blob;

		// check width
		if ( blob->box.width < laneComponent->minLaneWidth ) continue;

		sb::LanePart* lanePart = new sb::LanePart;
		lanePart->origin = blob->origin;
		lanePart->box = blob->box;
		lanePart->bgr = blob->bgr;

#ifdef SB_DEBUG_LANE_COMPONENT_FIND
		{
			cv::Mat img = frameInfo->bgrImage.clone();
			cv::rectangle( img, lanePart->box.tl(), lanePart->box.br(), cv::Scalar( 0, 0, 255 ), 2 );
			cv::circle( img, lanePart->origin, 3, cv::Scalar( 0, 255, 0 ), 2 );
			cv::rectangle( img, lastestLanePart->box.tl(), lastestLanePart->box.br(), cv::Scalar( 0, 0, 255 ), 2 );
			cv::circle( img, lastestLanePart->origin, 3, cv::Scalar( 0, 255, 0 ), 2 );
			cv::imshow( "Full lane", img );
			cv::waitKey();
		}
#endif

		// check pos diff
		float posDiff = static_cast<float>(abs( lanePart->origin.x - lastestLanePart->origin.x ));
		if ( posDiff > MAX_ACCEPTABLE_POSITION_DIFF_IN_LANE_PARTS ) {
			delete lanePart;
			continue;
		}

		// TODO: check width, angle diff

		// check color diff
		float colorDiff = static_cast<float>(sb::calculateDeltaE( lanePart->bgr, lastestLanePart->bgr ));
		if ( colorDiff > MAX_ACCEPTABLE_COLOR_DIFF_IN_LANE_PARTS ) {
			delete lanePart;
			continue;
		}

		float posRating = 10.0f * (MAX_ACCEPTABLE_POSITION_DIFF_IN_LANE_PARTS - posDiff)
				/ MAX_ACCEPTABLE_POSITION_DIFF_IN_LANE_PARTS;

		float colorRating = 10.0f * (MAX_ACCEPTABLE_COLOR_DIFF_IN_LANE_PARTS - colorDiff)
				/ MAX_ACCEPTABLE_COLOR_DIFF_IN_LANE_PARTS;

		// TODO: calculate width rating

		float rating = 0.4f * posRating + 0.6f * colorRating;

		nextLaneParts.push_back( std::make_pair( lanePart, rating ) );
	}
}

int sb::track( sb::LaneComponent* laneComponent, sb::FrameInfo* frameInfo )
{
	std::queue<sb::LanePartInfo*> poolParts; // for release memory

	std::vector<sb::LanePartInfo*> bestLaneParts; // final result
	float highestRating = 0;

	std::vector<std::vector<sb::LanePartInfo*>> lanePartsTrackResults; // tracked results
	lanePartsTrackResults.reserve( frameInfo->imageSections.size() );

	// track individual part
	{
		auto cit_part_info = laneComponent->parts.cbegin();
		auto cit_section = frameInfo->imageSections.cbegin();
		for ( ; cit_section != frameInfo->imageSections.cend(); ++cit_section , ++cit_part_info ) {
			sb::Section* section = *cit_section;

			std::vector<sb::LanePartInfo*> trackedParts;
			sb::trackIndividualPart( laneComponent, frameInfo, section, *cit_part_info, trackedParts );

			// not any part tracked, estimate a new one
			if ( trackedParts.empty() ) {

				sb::LanePartInfo* lanePartInfo = new sb::LanePartInfo;
				lanePartInfo->part = new sb::LanePart;

				// outsight left
				if ( (*cit_part_info)->part->origin.x < 50 ) {
					lanePartInfo->errorCode = sb::PART_OUTSIGHT_LEFT;
					lanePartInfo->part->origin = (*cit_part_info)->part->origin;
				}

				// outsight right
				else if ( (*cit_part_info)->part->origin.x > frameInfo->bgrImage.cols - 50 ) {
					lanePartInfo->errorCode = sb::PART_OUTSIGHT_RIGHT;
					lanePartInfo->part->origin = (*cit_part_info)->part->origin;
				}

				// unknown
				else {
					lanePartInfo->errorCode = sb::PART_UNKNOWN;
					lanePartInfo->part->origin = (*cit_part_info)->part->origin;
				}

				// TODO: errorCode == PART_OVERLAYED

				lanePartInfo->part->box = cv::Rect( lanePartInfo->part->origin.x - (*cit_part_info)->part->box.width / 2,
				                                    lanePartInfo->part->origin.y - (*cit_part_info)->part->box.height / 2,
				                                    (*cit_part_info)->part->box.width,
				                                    (*cit_part_info)->part->box.height );
				lanePartInfo->part->bgr = (*cit_part_info)->part->bgr;

				trackedParts.push_back( lanePartInfo );
			}

			// push to pool
			for ( auto cit_tracked_part = trackedParts.cbegin(); cit_tracked_part != trackedParts.cend(); ++cit_tracked_part )
				poolParts.push( *cit_tracked_part );

			lanePartsTrackResults.push_back( trackedParts );
		}
	}

	// combine results
	{
		std::queue<std::pair<std::vector<sb::LanePartInfo*>, float>> lanes;
		lanes.push( std::make_pair( std::vector<sb::LanePartInfo*>(), 0.0f ) );

		for ( auto cit_section_parts = lanePartsTrackResults.cbegin(); cit_section_parts != lanePartsTrackResults.cend(); ++cit_section_parts ) {
			size_t previousSize = lanes.size();
			while ( previousSize > 0 ) {
				std::vector<sb::LanePartInfo*> parts = lanes.front().first;
				float rating = lanes.front().second;
				lanes.pop();
				previousSize--;

				for ( auto cit_tracked_part = cit_section_parts->cbegin(); cit_tracked_part != cit_section_parts->cend(); ++cit_tracked_part ) {
					sb::LanePartInfo* trackedPart = *cit_tracked_part;

					std::vector<sb::LanePartInfo*> tempParts = parts;
					tempParts.push_back( trackedPart );

					float tempRating = rating;

					if ( parts.empty() ) {
						lanes.push( std::make_pair( tempParts, tempRating ) );
						continue;
					}

					sb::LanePartInfo* lastestPart = parts.back();

					// check pos diff
					float posRating = 0;
					float posDiff = MIN( MAX_ACCEPTABLE_POSITION_DIFF_IN_LANE_PARTS,
						static_cast<float>(abs( trackedPart->part->origin.x - lastestPart->part->origin.x )));
					posRating = 10.0f * (MAX_ACCEPTABLE_POSITION_DIFF_IN_LANE_PARTS - posDiff)
							/ MAX_ACCEPTABLE_POSITION_DIFF_IN_LANE_PARTS;

					// TODO: check width diff

					// TODO: check angle diff

					float colorRating = 0;
					float colorDiff = MIN( MAX_ACCEPTABLE_COLOR_DIFF_IN_LANE_PARTS,
						static_cast<float>(sb::calculateDeltaE( trackedPart->part->bgr, lastestPart->part->bgr )));
					colorRating = 10.0f * (MAX_ACCEPTABLE_COLOR_DIFF_IN_LANE_PARTS - colorDiff)
							/ MAX_ACCEPTABLE_COLOR_DIFF_IN_LANE_PARTS;

					tempRating += 0.3f * posRating + 0.7f * colorRating;

					// finish a sequence of lane parts, check for rating
					if ( tempParts.size() == frameInfo->imageSections.size() ) {
						tempRating /= tempParts.size() - 1;

						if ( tempRating > highestRating ) {
							bestLaneParts = tempParts;
							highestRating = tempRating;
						}
						continue;
					}

					lanes.push( std::make_pair( tempParts, tempRating ) );
				}
			}
		}
	}

	// move result to dedicated memory
	if ( highestRating > 0 ) {

		// copy best result
		auto cit_this_part = laneComponent->parts.cbegin();
		auto cit_tracked_part = bestLaneParts.cbegin();
		for ( ; cit_tracked_part != bestLaneParts.cend(); ++cit_tracked_part , ++cit_this_part ) {
			sb::LanePartInfo* thisPart = *cit_this_part;
			sb::LanePartInfo* trackedPart = *cit_tracked_part;

			*thisPart->part = *trackedPart->part;
			thisPart->errorCode = trackedPart->errorCode;
			thisPart->errorPosition = trackedPart->errorPosition;
			thisPart->errorWidth = trackedPart->errorWidth;
			thisPart->errorAngle = trackedPart->errorAngle;
			thisPart->errorColor = trackedPart->errorColor;
		}

		// fix error parts
		// TODO: fix error parts width common error position, previous diff position
	}

	// release pool
	while ( !poolParts.empty() ) {
		sb::release( poolParts.front() );
		delete poolParts.front();
		poolParts.pop();
	}

	return highestRating > 0.0f ? 0 : -1;
}

void sb::trackIndividualPart( sb::LaneComponent* laneComponent, sb::FrameInfo* frameInfo,
                              sb::Section* section, sb::LanePartInfo* oldPartInfo,
                              std::vector<sb::LanePartInfo*>& trackResults )
{
	// TODO: track in situations that errorCode == { PART_UNKNOWN, PART_OUTSIGHT_LEFT, PART_OUTSIGHTED_RIGHT, PART_OVERLAYED }

	for ( auto cit_blob = section->blobs.cbegin(); cit_blob != section->blobs.cend(); ++cit_blob ) {
		sb::Blob* blob = *cit_blob;

		// check width
		if ( blob->box.width < laneComponent->minLaneWidth ) continue;

		// create new part
		sb::LanePartInfo* newPartInfo = new sb::LanePartInfo;
		newPartInfo->part = new sb::LanePart;
		newPartInfo->part->origin = blob->origin;
		newPartInfo->part->box = blob->box;
		newPartInfo->part->bgr = blob->bgr;
		newPartInfo->errorAngle = 0.0f;
		newPartInfo->errorWidth = 0;
		newPartInfo->errorCode = sb::PART_NICE;

#ifdef SB_DEBUG_LANE_COMPONENT_TRACK
		{
			cv::Mat img = frameInfo->bgrImage.clone();
			cv::rectangle( img, oldPartInfo->part->box.tl(), oldPartInfo->part->box.br(), cv::Scalar( 0, 0, 255 ), 2 );
			cv::circle( img, oldPartInfo->part->origin, 3, cv::Scalar( 0, 255, 0 ), 2 );
			cv::imshow( "Old", img );
		} { 
			cv::Mat img = frameInfo->bgrImage.clone();
			cv::rectangle( img, newPartInfo->part->box.tl(), newPartInfo->part->box.br(), cv::Scalar( 0, 0, 255 ), 2 );
			cv::circle( img, newPartInfo->part->origin, 3, cv::Scalar( 0, 255, 0 ), 2 );
			cv::imshow( "New", img );
			cv::waitKey(500);
		}
#endif

		// check pos
		float posError = static_cast<float>(newPartInfo->part->origin.x - oldPartInfo->part->origin.x);
		if ( abs( posError ) > MAX_ACCEPTABLE_POSITION_ERROR_IN_TRACK_LANE_PART ) {
			sb::release( newPartInfo );
			delete newPartInfo;
			continue;
		}
		newPartInfo->errorPosition = static_cast<int>(posError);

		// TODO: calculate and check width error

		// TODO: calculate and check angle error

		// check color
		float colorError = static_cast<float>(sb::calculateDeltaE( newPartInfo->part->bgr, oldPartInfo->part->bgr ));
		if ( colorError > MAX_ACCEPTABLE_COLOR_ERROR_IN_TRACK_LANE_PART ) {
			sb::release( newPartInfo );
			delete newPartInfo;
			continue;
		}
		newPartInfo->errorColor = colorError;

		trackResults.push_back( newPartInfo );
	}
}

double sb::calculateDeltaE( const cv::Vec3f& bgr1, const cv::Vec3f& bgr2, double kL, double kC, double kH )
{
	cv::Vec3f lab1 = cvtBGRtoLab( bgr1 );
	cv::Vec3f lab2 = cvtBGRtoLab( bgr2 );

	double C1 = sqrt( lab1[1] * lab1[1] + lab1[2] * lab1[2] );
	double C2 = sqrt( lab2[1] * lab2[1] + lab2[2] * lab2[2] );

	double barC = (C1 + C2) / 2.0;

	double G = 0.5 * (1 - sqrt( pow( barC, 7 ) / (pow( barC, 7 ) + pow( 25, 7 )) ));

	double a1Prime = (1.0 + G) * lab1[1];
	double a2Prime = (1.0 + G) * lab2[1];

	double CPrime1 = sqrt( (a1Prime * a1Prime) + (lab1[2] * lab1[2]) );
	double CPrime2 = sqrt( (a2Prime * a2Prime) + (lab2[2] * lab2[2]) );

	double hPrime1;
	if ( lab1[2] == 0 && a1Prime == 0 )
		hPrime1 = 0.0;
	else {
		hPrime1 = atan2( lab1[2], a1Prime );

		if ( hPrime1 < 0 )
			hPrime1 += CV_2PI;
	}
	double hPrime2;
	if ( lab2[2] == 0 && a2Prime == 0 )
		hPrime2 = 0.0;
	else {
		hPrime2 = atan2( lab2[2], a2Prime );

		if ( hPrime2 < 0 )
			hPrime2 += CV_2PI;
	}

	double deltaLPrime = lab2[0] - lab1[0];

	double deltaCPrime = CPrime2 - CPrime1;

	double deltahPrime;
	double CPrimeProduct = CPrime1 * CPrime2;
	if ( CPrimeProduct == 0 )
		deltahPrime = 0;
	else {
		deltahPrime = hPrime2 - hPrime1;
		if ( deltahPrime < -CV_PI )
			deltahPrime += CV_2PI;
		else if ( deltahPrime > CV_PI )
			deltahPrime -= CV_2PI;
	}

	double deltaHPrime = 2.0 * sqrt( CPrimeProduct ) *
			sin( deltahPrime / 2.0 );

	double barLPrime = (lab1[0] + lab2[0]) / 2.0;

	double barCPrime = (CPrime1 + CPrime2) / 2.0;

	double barhPrime, hPrimeSum = hPrime1 + hPrime2;
	if ( CPrime1 * CPrime2 == 0 ) {
		barhPrime = hPrimeSum;
	}
	else {
		if ( fabs( hPrime1 - hPrime2 ) <= CV_PI )
			barhPrime = hPrimeSum / 2.0;
		else {
			if ( hPrimeSum < CV_2PI )
				barhPrime = (hPrimeSum + CV_2PI) / 2.0;
			else
				barhPrime = (hPrimeSum - CV_2PI) / 2.0;
		}
	}

	double T = 1.0 - (0.17 * cos( barhPrime - (CV_PI * 30 / 180) )) +
			(0.24 * cos( 2.0 * barhPrime )) +
			(0.32 * cos( (3.0 * barhPrime) + (CV_PI * 6 / 180) )) -
			(0.20 * cos( (4.0 * barhPrime) - (CV_PI * 63 / 180) ));

	double deltaTheta = (CV_PI * 30 / 180) *
			exp( -pow( (barhPrime - (CV_PI * 275 / 180)) / (CV_PI * 25 / 180), 2.0 ) );

	double R_C = 2.0 * sqrt( pow( barCPrime, 7.0 ) /
	                        (pow( barCPrime, 7.0 ) + pow( 25, 7 )) );

	double S_L = 1 + ((0.015 * pow( barLPrime - 50.0, 2.0 )) /
		sqrt( 20 + pow( barLPrime - 50.0, 2.0 ) ));

	double S_C = 1 + (0.045 * barCPrime);

	double S_H = 1 + (0.015 * barCPrime * T);

	double R_T = (-sin( 2.0 * deltaTheta )) * R_C;

	double deltaE = sqrt(
	                     pow( deltaLPrime / (kL * S_L), 2.0 ) +
	                     pow( deltaCPrime / (kC * S_C), 2.0 ) +
	                     pow( deltaHPrime / (kH * S_H), 2.0 ) +
	                     (R_T * (deltaCPrime / (kC * S_C)) * (deltaHPrime / (kH * S_H))) );

	return deltaE;
}

cv::Vec3f sb::cvtBGRtoLab( const cv::Vec3b& bgr )
{
	cv::Mat3f bgrMat = cv::Mat3f::zeros( 1, 1 );
	bgrMat.at<cv::Vec3f>( 0, 0 ) = (cv::Vec3f( bgr.val[0] / 255.0f, bgr.val[1] / 255.0f, bgr.val[2] / 255.0f ));

	cv::Mat3f lab;
	cvtColor( bgrMat, lab, cv::COLOR_BGR2Lab );

	return lab.at<cv::Vec3f>( cv::Point( 0, 0 ) );
}
