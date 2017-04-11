#include "LaneComponent.h"
#include "../collector/Collector.h"
#include "Lane.h"
#include <queue>

const std::vector<sb::LanePartInfo>& sb::LaneComponent::getParts() const { return _parts; }

void sb::LaneComponent::init( int side )
{
	_side = side;

	_minLaneWidth = 20;

	_maxLaneWidth = 60;

	_windowWidth = _maxLaneWidth * 2;

	_windowMove = _maxLaneWidth;
}

int sb::LaneComponent::find( const sb::FrameInfo& frameInfo )
{
	double minX = -_windowWidth;
	double maxX = 1.0 * frameInfo.getColorImage().cols + _windowWidth;
	double centerX = (minX + maxX) * 0.5;

	// result highest lane parts
	double highestRating = 0;
	_parts.assign( frameInfo.getImageSections().size(), sb::LanePartInfo() );

	// window
	double windowX = _side < 0 ? centerX - _windowWidth : centerX;

	// move window around section to find possible lane
	while ( windowX >= minX && windowX + _windowWidth <= maxX ) {

#ifdef SB_DEBUG
		debugImages[0] = frameInfo.getColorImage().clone();
		debugImages[1] = frameInfo.getEdgesImage().clone();
		debugImages[2] = cv::Mat::zeros( frameInfo.getColorImage().size(), CV_8UC3 );
		for ( auto it_section = frameInfo.getImageSections().cbegin();
		      it_section != frameInfo.getImageSections().cend(); ++it_section ) {
			for ( auto it_line = it_section->getImageLines().cbegin();
			      it_line != it_section->getImageLines().cend(); ++it_line )
				cv::line( debugImages[2], it_line->getStartingPoint(), it_line->getEndingPoint(),
				          cv::Scalar( 255, 255, 255 ) );
		}
		cv::cvtColor( debugImages[1], debugImages[1], cv::COLOR_GRAY2BGR );
#endif // init images

		/*#ifdef SB_DEBUG
				for ( int i = 0; i < 3; i++ ) {
					cv::rectangle( debugImages[i],
					               cv::Point2d( windowX, frameInfo.getImageSections().front().getImageRect().tl().y ),
					               cv::Point2d( windowX + _windowWidth, frameInfo.getImageSections().front().getImageRect().br().y ),
					               cv::Scalar( 0, 0, 255 ), 1 );
					cv::imshow( "Image " + std::to_string( i ), debugImages[i] );
				}
				cv::waitKey( 100 );
		#endif*/ // show and draw window

		std::vector<sb::LanePart> firstLaneParts;
		findFirstLaneParts( frameInfo.getImageSections().front(), frameInfo.getColorImage(), windowX, firstLaneParts );

		for ( const auto& firstLanePart : firstLaneParts ) {
			std::vector<sb::LanePart> fullLaneParts;
			double fullLaneRating;

			findBestFullLaneParts( frameInfo.getImageSections(), frameInfo.getColorImage(), firstLanePart, fullLaneParts, fullLaneRating );

			/*#ifdef SB_DEBUG
						for ( int i = 0; i < 3; i++ ) {
							cv::Mat img = debugImages[i].clone();
							for ( const auto& part : fullLaneParts ) {
								drawLanePart( img, part );
							}
							cv::imshow( "Image " + std::to_string( i ), img );
							cv::waitKey( 500 );
						}
			#endif*/ //show completed lane

			if ( fullLaneRating > highestRating ) {
				auto it_part_info = _parts.begin();
				auto it_part = fullLaneParts.cbegin();
				for ( ; it_part != fullLaneParts.cend(); ++it_part , ++it_part_info ) {
					it_part_info->part = *it_part;
				}
				highestRating = fullLaneRating;
			}
		}

		// move window
		windowX += _side < 0 ? -_windowMove : _windowMove;

	}

	if ( highestRating < 3.0 ) return -1;

	for ( auto it_part_info = _parts.begin(); it_part_info != _parts.end(); ++it_part_info ) {
		it_part_info->errorAngle = 0;
		it_part_info->errorPosition = 0;
		it_part_info->errorWidth = 0;
		it_part_info->errorColor = 0;
		it_part_info->errorCode = sb::BOTH_LINE_ATTACHED;
	}

	return 0;
}

int sb::LaneComponent::track( const sb::FrameInfo& frameInfo )
{
	std::vector<sb::LanePartInfo> bestLaneParts;
	double highestRating = 0;

	std::vector<std::vector<sb::LanePartInfo>> lanePartsTrackResults;
	lanePartsTrackResults.reserve( frameInfo.getImageSections().size() );

	// TODO: scenario that old part was error

	// track individual lane part
	{
		auto it_section = frameInfo.getImageSections().cbegin();
		auto it_part = _parts.cbegin();

		for ( ; it_section != frameInfo.getImageSections().cend(); ++it_section , ++it_part ) {
			size_t index = std::distance( frameInfo.getImageSections().cbegin(), it_section );

			std::vector<sb::LanePartInfo> laneParts;

			// TODO: scenario that old part was error

			int rc;

			rc = trackIndividualLanePart_PlanA( it_part->part, *it_section, frameInfo.getColorImage(), frameInfo.getEdgesImage(), laneParts );

			// if ( rc < 0 ) rc = trackIndividualLanePart_PlanB( it_part->part, *it_section, frameInfo.getColorImage(), frameInfo.getEdgesImage(), laneParts );

			// if ( rc < 0 ) rc = trackIndividualLanePart_PlanC( it_part->part, *it_section, frameInfo.getColorImage(), frameInfo.getEdgesImage(), laneParts );

			// if ( rc < 0 ) rc = trackIndividualLanePart_PlanD( it_part->part, *it_section, frameInfo.getColorImage(), frameInfo.getEdgesImage(), laneParts );

			if ( laneParts.empty() ) {
				sb::LanePartInfo emptyLanePart;
				emptyLanePart.errorCode = sb::UNKNOWN;

				laneParts.push_back( emptyLanePart );
			}

			lanePartsTrackResults.push_back( laneParts );
		}
	}

	// combine results
	{
		std::queue<std::pair<std::vector<sb::LanePartInfo>, double>> lanes;
		lanes.push( std::make_pair( std::vector<sb::LanePartInfo>(), 0 ) );

		// trav all result combinations
		for ( auto it_section_parts = lanePartsTrackResults.cbegin(); it_section_parts != lanePartsTrackResults.cend(); ++it_section_parts ) {
			size_t previousSize = lanes.size();
			while ( previousSize > 0 ) {
				auto lane = lanes.front();
				lanes.pop();
				previousSize--;

				for ( auto it_tracked_part = it_section_parts->cbegin(); it_tracked_part != it_section_parts->cend(); ++it_tracked_part ) {
					std::vector<sb::LanePartInfo> tempParts( lane.first.cbegin(), lane.first.cend() );
					tempParts.push_back( *it_tracked_part );

					double tempRating = lane.second;

					// compare two adjacent lane part to calculate rating
					if ( !lane.first.empty() ) {

						sb::LanePartInfo lastestPart = lane.first.back();

						double posRating = 0;
						if ( it_tracked_part->errorCode != sb::UNKNOWN ) {
							double posDiff = MIN( MAX_ACCEPTABLE_POSITION_DIFF_BETWEEN_ADJACENT_LANE_PARTS,
								abs( it_tracked_part->part.innerLine.getBottomPoint().x - lastestPart.part.innerLine.getTopPoint().x ) );

							posRating = 10.0 * (MAX_ACCEPTABLE_POSITION_DIFF_BETWEEN_ADJACENT_LANE_PARTS - posDiff)
									/ MAX_ACCEPTABLE_POSITION_DIFF_BETWEEN_ADJACENT_LANE_PARTS;
						}

						double widthRating = 0;
						if ( it_tracked_part->errorCode != sb::UNKNOWN ) {
							double lastestWidth = abs( lastestPart.part.innerLine.getTopPoint().x - lastestPart.part.outerLine.getTopPoint().x );
							double width = abs( it_tracked_part->part.innerLine.getBottomPoint().x - it_tracked_part->part.outerLine.getBottomPoint().x );

							double widthDiff = MIN( MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_ADJACENT_LANE_PARTS,
								abs( width - lastestWidth ) );

							widthRating = 10.0 * (MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_ADJACENT_LANE_PARTS - widthDiff)
									/ MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_ADJACENT_LANE_PARTS;
						}

						double colorRating = 0;
						if ( it_tracked_part->errorCode != sb::UNKNOWN ) {
							double colorDiff = MIN( MAX_ACCEPTABLE_COLOR_DIFF_BETWEEN_ADJACENT_LANE_PARTS,
								calculateDeltaE( it_tracked_part->part.laneColor, lastestPart.part.laneColor ) );

							colorRating = 10.0 * (MAX_ACCEPTABLE_COLOR_DIFF_BETWEEN_ADJACENT_LANE_PARTS - colorDiff)
									/ MAX_ACCEPTABLE_COLOR_DIFF_BETWEEN_ADJACENT_LANE_PARTS;
						}

						tempRating += 0.3 * posRating + 0.4 * widthRating + 0.3 * colorRating;
					}

					// finish a sequence of lane parts, check for rating
					if ( tempParts.size() == frameInfo.getImageSections().size() ) {
						tempRating /= tempParts.size() - 1;

						if ( tempRating > highestRating ) {
							bestLaneParts = tempParts;
							highestRating = tempRating;
						}
					}
					else {
						lanes.push( std::make_pair( tempParts, tempRating ) );
					}

				}
			}
		}
	}

	_parts = bestLaneParts;

	return 0;
}

void sb::LaneComponent::findFirstLaneParts( const sb::Section& firstSection, const cv::Mat& colorImage, double windowX,
                                            std::vector<sb::LanePart>& firstLaneParts )
{
	firstLaneParts.clear();

	// find lines in window
	std::vector<sb::LineInfo> lines;
	for ( auto it_line = firstSection.getImageLines().cbegin();
	      it_line != firstSection.getImageLines().cend(); ++it_line ) {
		if ( it_line->getBottomPoint().x >= windowX && it_line->getBottomPoint().x <= windowX + _windowWidth )
			lines.push_back( *it_line );
	}

	// find line couples contribute lane part
	for ( auto it_line_1 = lines.cbegin();
	      it_line_1 != lines.cend(); ++it_line_1 ) {

		for ( auto it_line_2 = it_line_1 + 1;
		      it_line_2 != lines.cend(); ++it_line_2 ) {

			///// Lane part criterias /////
			double width = it_line_2->getBottomPoint().x - it_line_1->getBottomPoint().x;

			// check width
			if ( width < _minLaneWidth || width > _maxLaneWidth ) continue;

			// check top knots
			double width2nd = it_line_2->getTopPoint().x - it_line_1->getTopPoint().x;
			if ( width2nd < 0 || width2nd - width > 2 ) continue;
			///// Lane part criterias /////

			sb::LanePart lanePart;
			if ( _side < 0 ) {
				lanePart.innerLine = *it_line_2;
				lanePart.outerLine = *it_line_1;
			}
			else {
				lanePart.innerLine = *it_line_1;
				lanePart.outerLine = *it_line_2;
			}

			bool hasSimilarPart = false;
			for ( auto it_old_part = firstLaneParts.cbegin();
			      it_old_part != firstLaneParts.cend(); ++it_old_part ) {
				if ( abs( lanePart.innerLine.getBottomPoint().x - it_old_part->innerLine.getBottomPoint().x ) < POSITION_THRESH_FOR_SIMILAR_LANE
					&& abs( lanePart.outerLine.getBottomPoint().x - it_old_part->outerLine.getBottomPoint().x ) < POSITION_THRESH_FOR_SIMILAR_LANE
					&& abs( lanePart.innerLine.getTopPoint().x - it_old_part->innerLine.getTopPoint().x ) < POSITION_THRESH_FOR_SIMILAR_LANE
					&& abs( lanePart.outerLine.getTopPoint().x - it_old_part->outerLine.getTopPoint().x ) < POSITION_THRESH_FOR_SIMILAR_LANE ) {
					hasSimilarPart = true;
					break;
				}
			}

			if ( hasSimilarPart ) continue;

			getPartColor( colorImage, lanePart );

			double distanceToWhite = calculateDeltaE( lanePart.laneColor, cv::Vec3b( 255, 255, 255 ) );
			if ( distanceToWhite > MAX_ACCEPTABLE_DISTANCE_TO_WHITE_COLOR ) continue;

			firstLaneParts.push_back( lanePart );
		}
	}
}

void sb::LaneComponent::findNextLaneParts( const sb::Section& section, const cv::Mat& colorImage,
                                           const sb::LanePart& lastestLanePart,
                                           std::vector<std::pair<sb::LanePart, double>>& nextLaneParts )
{
	nextLaneParts.clear();

	std::vector<sb::LineInfo> lines;

	// find possible lines
	for ( auto it_line = section.getImageLines().cbegin();
	      it_line != section.getImageLines().cend(); ++it_line ) {

		double posDiff1 = abs( it_line->getBottomPoint().x - lastestLanePart.innerLine.getTopPoint().x );
		double posDiff2 = abs( it_line->getBottomPoint().x - lastestLanePart.outerLine.getTopPoint().x );

		double width = abs( lastestLanePart.innerLine.getTopPoint().x - lastestLanePart.outerLine.getTopPoint().x );
		if ( posDiff1 > width * 0.5 && posDiff2 > width * 0.5 ) continue;

		double angleDiff1 = abs( lastestLanePart.innerLine.getAngle() - it_line->getAngle() );
		double angleDiff2 = abs( lastestLanePart.outerLine.getAngle() - it_line->getAngle() );
		if ( angleDiff1 > MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_ADJACENT_LANE_PARTS
			&& angleDiff2 > MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_ADJACENT_LANE_PARTS )
			continue;

		lines.push_back( *it_line );

	}

	// TODO: xét trường hợp có một line, nhưng line đó khớp cao -> thêm vào + cộng điểm

	// find line couples contribute lane part
	for ( auto it_line_1 = lines.cbegin();
	      it_line_1 != lines.cend(); ++it_line_1 ) {

		for ( auto it_line_2 = it_line_1 + 1;
		      it_line_2 != lines.cend(); ++it_line_2 ) {

			double width = it_line_2->getBottomPoint().x - it_line_1->getBottomPoint().x;

			// check width
			if ( width < _minLaneWidth || width > _maxLaneWidth ) continue;

			// check top knots
			double width2nd = it_line_2->getTopPoint().x - it_line_1->getTopPoint().x;
			if ( width2nd < 0 || width2nd - width > 2 ) continue;

			sb::LanePart lanePart;
			if ( _side < 0 ) {
				lanePart.innerLine = *it_line_2;
				lanePart.outerLine = *it_line_1;
			}
			else {
				lanePart.innerLine = *it_line_1;
				lanePart.outerLine = *it_line_2;
			}

			bool hasSimilarPart = false;
			for ( auto it_old_part = nextLaneParts.cbegin();
			      it_old_part != nextLaneParts.cend(); ++it_old_part ) {
				if ( abs( lanePart.innerLine.getBottomPoint().x - it_old_part->first.innerLine.getBottomPoint().x ) < 2
					&& abs( lanePart.outerLine.getBottomPoint().x - it_old_part->first.outerLine.getBottomPoint().x ) < 2
					&& abs( lanePart.innerLine.getTopPoint().x - it_old_part->first.innerLine.getTopPoint().x ) < 2
					&& abs( lanePart.outerLine.getTopPoint().x - it_old_part->first.outerLine.getTopPoint().x ) < 2 ) {
					hasSimilarPart = true;
					break;
				}
			}

			if ( hasSimilarPart ) continue;

			getPartColor( colorImage, lanePart );

			double colorDiff = calculateDeltaE( lanePart.laneColor, lastestLanePart.laneColor );
			if ( colorDiff > MAX_ACCEPTABLE_COLOR_DIFF_BETWEEN_ADJACENT_LANE_PARTS )break;

			double angleDiffWithLastest = abs( lastestLanePart.innerLine.getAngle() - lanePart.innerLine.getAngle() );
			if ( angleDiffWithLastest > MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_ADJACENT_LANE_PARTS ) continue;

			double lastestWidth = abs( lastestLanePart.innerLine.getTopPoint().x - lastestLanePart.outerLine.getTopPoint().x );

			double widthDiffWithLastest = abs( width - lastestWidth );
			if ( widthDiffWithLastest > MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_ADJACENT_LANE_PARTS ) continue;

			double posDiffWithLastest = abs( lastestLanePart.innerLine.getTopPoint().x - lanePart.innerLine.getBottomPoint().x );
			if ( posDiffWithLastest > MAX_ACCEPTABLE_POSITION_DIFF_BETWEEN_ADJACENT_LANE_PARTS ) continue;

			double widthRating = 10.0 * (MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_ADJACENT_LANE_PARTS - widthDiffWithLastest)
					/ MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_ADJACENT_LANE_PARTS;
			double posRating = 10.0 * (MAX_ACCEPTABLE_POSITION_DIFF_BETWEEN_ADJACENT_LANE_PARTS - posDiffWithLastest)
					/ MAX_ACCEPTABLE_POSITION_DIFF_BETWEEN_ADJACENT_LANE_PARTS;

			double colorRating = 10.0 * (MAX_ACCEPTABLE_COLOR_DIFF_BETWEEN_ADJACENT_LANE_PARTS - colorDiff)
					/ MAX_ACCEPTABLE_COLOR_DIFF_BETWEEN_ADJACENT_LANE_PARTS;

			double rating = 0.4 * posRating + 0.4 * widthRating + 0.2 * colorRating;
			nextLaneParts.push_back( std::pair<sb::LanePart, double>( lanePart, rating ) );
		}
	}
}

void sb::LaneComponent::findBestFullLaneParts( const std::vector<sb::Section>& sections, const cv::Mat& colorImage,
                                               const sb::LanePart& firstLanePart,
                                               std::vector<sb::LanePart>& fullLaneParts,
                                               double& fullLaneRating )
{
	fullLaneRating = 0;

	std::stack<std::pair<std::vector<sb::LanePart>, double>> stackParts;

	// init lane
	{
		std::pair<std::vector<sb::LanePart>, double> tempParts;
		tempParts.first.reserve( sections.size() );
		tempParts.first.push_back( firstLanePart );
		tempParts.second = 0;

		stackParts.push( tempParts );
	}

	while ( !stackParts.empty() ) {

		// pop a bufferred scenario, find and push it new lane part
		std::pair<std::vector<sb::LanePart>, double> parts = stackParts.top();
		stackParts.pop();

		/*#ifdef SB_DEBUG
				{
					for ( int i = 0; i < 3; i++ ) {
						cv::Mat img = debugImages[i].clone();
		
						for ( const auto& part : parts.first ) {
							drawLanePart( img, part );
						}
		
						cv::imshow( "Image " + std::to_string( i ), img );
						cv::waitKey( 200 );
					}
				}
		#endif*/ //show and sequence of lane parts

		// finish a lane
		if ( parts.first.size() == sections.size() ) {
			parts.second /= sections.size() - 1;

			if ( parts.second > fullLaneRating ) {

				/*#ifdef SB_DEBUG
								for ( int i = 0; i < 3; i++ ) {
									cv::Mat img = debugImages[i].clone();
				
									for ( const auto& part : parts.first ) {
										drawLanePart( img, part );
									}
				
									std::stringstream stringBuilder;
									stringBuilder << "Rating: " << parts.second;
									cv::putText( img, stringBuilder.str(), cv::Point2d( 30, 30 ), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ) );
									cv::imshow( "Image " + std::to_string( i ), img );
									cv::waitKey( 300 );
								}
				#endif*/ //show completed lane

				fullLaneParts = parts.first;
				fullLaneRating = parts.second;

			}

			continue;
		}

		// lastest lane part
		sb::LanePart lastestPart = parts.first.back();

		std::vector<std::pair<sb::LanePart, double>> nextLaneParts;
		findNextLaneParts( sections[parts.first.size()], colorImage, lastestPart, nextLaneParts );

		// TODO: thêm mẫu rỗng khi next lane part rỗng
		// if none of next part found
		if ( nextLaneParts.empty() ) continue;

		// push new lanes in to stack
		for ( auto it_next_part = nextLaneParts.cbegin();
		      it_next_part != nextLaneParts.cend(); ++it_next_part ) {
			std::vector<sb::LanePart> tempParts( parts.first.cbegin(), parts.first.cend() );
			tempParts.push_back( it_next_part->first );

			double tempRating = parts.second + it_next_part->second;
			stackParts.push( std::make_pair( tempParts, tempRating ) );
		}
	}
}

int sb::LaneComponent::trackIndividualLanePart_PlanA( const sb::LanePart& part,
                                                      const sb::Section& section,
                                                      const cv::Mat& colorImage,
                                                      const cv::Mat& edgesImage,
                                                      std::vector<sb::LanePartInfo>& trackResults )
{
	if ( _side < 0 ) {
		double previousWidth = part.innerLine.getBottomPoint().x - part.outerLine.getBottomPoint().x;

		for ( auto it_inner = section.getImageLines().crbegin();
		      it_inner != section.getImageLines().crend(); ++it_inner ) {

			double angleError = it_inner->getAngle() - part.innerLine.getAngle();
			if ( abs( angleError ) > MAX_ACCEPTABLE_ANGLE_ERROR_TRACK_LINE ) continue;

			double posError = it_inner->getBottomPoint().x - part.innerLine.getBottomPoint().x;
			if ( abs( posError ) > MAX_ACCEPTABLE_POSITION_ERROR_TRACK_LINE ) continue;

			for ( auto it_outer = it_inner + 1; it_outer != section.getImageLines().crend(); ++it_outer ) {

				double width = it_inner->getBottomPoint().x - it_outer->getBottomPoint().x;
				if ( width < _minLaneWidth || width > _maxLaneWidth ) continue;

				double width2nd = it_inner->getTopPoint().x - it_outer->getTopPoint().x;
				if ( width2nd < 0 || width2nd - width > 2 ) continue;

				double widthError = abs( width - previousWidth ) / previousWidth;
				if ( widthError > MAX_ACCEPTABLE_WIDTH_ERROR_TRACK_LANE ) continue;

				bool hasSimilarPart = false;
				for ( auto it_old_result = trackResults.cbegin();
				      it_old_result != trackResults.cend(); ++it_old_result ) {
					if ( abs( it_inner->getBottomPoint().x - it_old_result->part.innerLine.getBottomPoint().x ) < POSITION_THRESH_FOR_SIMILAR_LANE
						&& abs( it_outer->getBottomPoint().x - it_old_result->part.outerLine.getBottomPoint().x ) < POSITION_THRESH_FOR_SIMILAR_LANE
						&& abs( it_inner->getTopPoint().x - it_old_result->part.innerLine.getTopPoint().x ) < POSITION_THRESH_FOR_SIMILAR_LANE
						&& abs( it_outer->getTopPoint().x - it_old_result->part.outerLine.getTopPoint().x ) < POSITION_THRESH_FOR_SIMILAR_LANE ) {
						hasSimilarPart = true;
						break;
					}
				}
				if ( hasSimilarPart ) continue;

				cv::Vec3b laneColor = getColorBetweenLines( colorImage, *it_inner, *it_outer );
				double colorError = calculateDeltaE( laneColor, part.laneColor );
				if ( colorError > MAX_ACCEPTABLE_COLOR_ERROR_TRACK_LANE ) continue;

				sb::LanePartInfo lanePartInfo;
				lanePartInfo.part.innerLine = *it_inner;
				lanePartInfo.part.outerLine = *it_outer;
				lanePartInfo.part.laneColor = laneColor;
				lanePartInfo.errorPosition = posError;
				lanePartInfo.errorAngle = angleError;
				lanePartInfo.errorWidth = widthError;
				lanePartInfo.errorColor = colorError;
				lanePartInfo.errorCode = sb::BOTH_LINE_ATTACHED;

#ifdef SB_DEBUG
				for ( int i = 0; i < 3; i++ ) {
					cv::Mat img = debugImages[i].clone();
					drawLanePart( img, lanePartInfo.part );
					cv::imshow( "Image " + std::to_string( i ), img );
				}
				cv::waitKey();
#endif

				trackResults.push_back( lanePartInfo );

			}

		}
	}
	else {
		double previousWidth = part.innerLine.getBottomPoint().x - part.outerLine.getBottomPoint().x;

		for ( auto it_inner = section.getImageLines().cbegin();
		      it_inner != section.getImageLines().cend(); ++it_inner ) {

			double angleError = it_inner->getAngle() - part.innerLine.getAngle();
			if ( abs( angleError ) > MAX_ACCEPTABLE_ANGLE_ERROR_TRACK_LINE ) continue;

			double posError = it_inner->getBottomPoint().x - part.innerLine.getBottomPoint().x;
			if ( abs( posError ) > MAX_ACCEPTABLE_POSITION_ERROR_TRACK_LINE ) continue;

			for ( auto it_outer = it_inner + 1; it_outer != section.getImageLines().cend(); ++it_outer ) {

				double width = it_outer->getBottomPoint().x - it_inner->getBottomPoint().x;
				if ( width < _minLaneWidth || width > _maxLaneWidth ) continue;

				double width2nd = it_outer->getTopPoint().x - it_inner->getTopPoint().x;
				if ( width2nd < 0 || width2nd - width > 2 ) continue;

				double widthError = abs( width - previousWidth ) / previousWidth;
				if ( widthError > MAX_ACCEPTABLE_WIDTH_ERROR_TRACK_LANE ) continue;

				bool hasSimilarPart = false;
				for ( auto it_old_result = trackResults.cbegin();
				      it_old_result != trackResults.cend(); ++it_old_result ) {
					if ( abs( it_inner->getBottomPoint().x - it_old_result->part.innerLine.getBottomPoint().x ) < POSITION_THRESH_FOR_SIMILAR_LANE
						&& abs( it_outer->getBottomPoint().x - it_old_result->part.outerLine.getBottomPoint().x ) < POSITION_THRESH_FOR_SIMILAR_LANE
						&& abs( it_inner->getTopPoint().x - it_old_result->part.innerLine.getTopPoint().x ) < POSITION_THRESH_FOR_SIMILAR_LANE
						&& abs( it_outer->getTopPoint().x - it_old_result->part.outerLine.getTopPoint().x ) < POSITION_THRESH_FOR_SIMILAR_LANE ) {
						hasSimilarPart = true;
						break;
					}
				}
				if ( hasSimilarPart ) continue;

				cv::Vec3b laneColor = getColorBetweenLines( colorImage, *it_inner, *it_outer );
				double colorError = calculateDeltaE( laneColor, part.laneColor );
				if ( colorError > MAX_ACCEPTABLE_COLOR_ERROR_TRACK_LANE ) continue;

				sb::LanePartInfo lanePartInfo;
				lanePartInfo.part.innerLine = *it_inner;
				lanePartInfo.part.outerLine = *it_outer;
				lanePartInfo.part.laneColor = laneColor;
				lanePartInfo.errorPosition = posError;
				lanePartInfo.errorAngle = angleError;
				lanePartInfo.errorWidth = widthError;
				lanePartInfo.errorColor = colorError;
				lanePartInfo.errorCode = sb::BOTH_LINE_ATTACHED;

				trackResults.push_back( lanePartInfo );

#ifdef SB_DEBUG
				for ( int i = 0; i < 3; i++ ) {
					cv::Mat img = debugImages[i].clone();
					drawLanePart( img, lanePartInfo.part );
					cv::imshow( "Image " + std::to_string( i ), img );
				}
				cv::waitKey();
#endif
			}

		}
	}

	// TODO: return -1 if too few results received
	if ( trackResults.empty() ) return -1;

	return 0;
}

int sb::LaneComponent::trackIndividualLanePart_PlanB( const sb::LanePart& part,
                                                      const sb::Section& section,
                                                      const cv::Mat& colorImage,
                                                      const cv::Mat& edgesImage,
                                                      std::vector<sb::LanePartInfo>& trackResults )
{
	// TODO: estimate outer line base on color

	// TODO: return 0 if there're some good result

	// TODO: return -1 if too few results received

	return 0;
}

int sb::LaneComponent::trackIndividualLanePart_PlanC( const sb::LanePart& part,
                                                      const sb::Section& section,
                                                      const cv::Mat& colorImage,
                                                      const cv::Mat& edgesImage,
                                                      std::vector<sb::LanePartInfo>& trackResults )
{
	// TODO: estimate inner line base on color

	for ( auto it_outer = section.getImageLines().cbegin();
	      it_outer != section.getImageLines().cend(); ++it_outer ) {

		double angleError = it_outer->getAngle() - part.outerLine.getAngle();
		if ( abs( angleError ) > MAX_ACCEPTABLE_ANGLE_ERROR_TRACK_LINE ) continue;

		double posError = it_outer->getBottomPoint().x - part.outerLine.getBottomPoint().x;
		if ( abs( posError ) > MAX_ACCEPTABLE_POSITION_ERROR_TRACK_LINE ) continue;

		// TODO: color check

		// TODO: calculate rating

		sb::LanePartInfo lanePartInfo;

		// TODO: set lane part info
		// TODO: set error state is INNER_LINE_LOST

		trackResults.push_back( lanePartInfo );
	}

	// TODO: return 0 if there're some good result

	// TODO: return -1 if too few results received

	return 0;
}

int sb::LaneComponent::trackIndividualLanePart_PlanD( const sb::LanePart& part,
                                                      const sb::Section& section,
                                                      const cv::Mat& colorImage,
                                                      const cv::Mat& edgesImage,
                                                      std::vector<sb::LanePartInfo>& trackResults )
{
	// TODO: estimate inner and outer line base on color

	// TODO: color check

	// TODO: calculate rating

	sb::LanePartInfo lanePartInfo;

	// TODO: set lane part info
	// TODO: set error state is BOTH_LINE_LOST

	trackResults.push_back( lanePartInfo );

	// TODO: return 0 if there're some good result

	// TODO: return -1 if too few results received

	return 0;
}

void sb::LaneComponent::getPartColor( const cv::Mat& colorImage, sb::LanePart& part )
{
	part.laneColor = getColorBetweenLines( colorImage, part.innerLine, part.outerLine );
}

cv::Vec3b sb::LaneComponent::getColorBetweenLines( const cv::Mat& colorImage, const sb::LineInfo& l1, const sb::LineInfo l2 )
{
	cv::Point rectPoints[4] = {
		cv::Point( static_cast<int>(MIN( colorImage.cols - 1, MAX( 0, l1.getTopPoint().x ) )),
		           static_cast<int>(MIN( colorImage.rows - 1, MAX( 0, l1.getTopPoint().y ) )) ),
		cv::Point( static_cast<int>(MIN( colorImage.cols - 1, MAX( 0, l1.getBottomPoint().x ) )),
		           static_cast<int>(MIN( colorImage.rows - 1, MAX( 0,l1.getBottomPoint().y ) )) ),
		cv::Point( static_cast<int>(MIN( colorImage.cols - 1, MAX( 0, l2.getBottomPoint().x ) )),
		           static_cast<int>(MIN( colorImage.rows - 1, MAX( 0,l2.getBottomPoint().y ) )) ) ,
		cv::Point( static_cast<int>(MIN( colorImage.cols - 1, MAX( 0, l2.getTopPoint().x ) )),
		           static_cast<int>(MIN( colorImage.rows - 1, MAX( 0,l2.getTopPoint().y ) )) ) };

	return getMainColor( colorImage, rectPoints );
}

cv::Vec3b sb::LaneComponent::getMainColor( const cv::Mat image, const cv::Point rectPoints[4] )
{
	cv::Point top = rectPoints[0];
	cv::Point bot = rectPoints[0];
	cv::Point left = rectPoints[0];
	cv::Point right = rectPoints[0];

	for ( int i = 1; i <= 3; i++ ) {
		if ( rectPoints[i].y < top.y ) {
			top = rectPoints[i];
		}

		if ( rectPoints[i].y > bot.y ) {
			bot = rectPoints[i];
		}

		if ( rectPoints[i].x < left.x ) {
			left = rectPoints[i];
		}

		if ( rectPoints[i].x > right.x ) {
			right = rectPoints[i];
		}
	}

	sb::Line tl( top, left );
	sb::Line tr( top, right );
	sb::Line bl( bot, left );
	sb::Line br( bot, right );

	sb::Line currentLeft = tl;
	sb::Line currentRight = tr;

	int bImage[64]{ 0 };
	int gImage[64]{ 0 };
	int rImage[64]{ 0 };

	for ( int i = top.y; i <= bot.y; i++ ) {

		sb::Line horizon( cv::Point2d( 0, i ), cv::Point2d( 1, i ) );
		cv::Point2d pl, pr;
		sb::Line::findIntersection( horizon, currentLeft, pl );
		sb::Line::findIntersection( horizon, currentRight, pr );

		int l = static_cast<int>(pl.x);
		int r = static_cast<int>(pr.x);

		for ( int j = l; j <= r; j++ ) {
			cv::Vec3b pixel = image.at<cv::Vec3b>( cv::Point( j, i ) );
			bImage[pixel.val[0] / 4]++;
			gImage[pixel.val[1] / 4]++;
			rImage[pixel.val[2] / 4]++;
		}

		if ( i > currentLeft.getEndingPoint().y ) {
			currentLeft = bl;
		}

		if ( i > currentRight.getEndingPoint().y ) {
			currentRight = br;
		}
	}

	int bMax = 0;
	int gMax = 0;
	int rMax = 0;

	for ( int i = 1; i < 64; i++ ) {
		if ( bImage[i] > bImage[bMax] ) {
			bMax = i;
		}

		if ( gImage[i] > gImage[gMax] ) {
			gMax = i;
		}

		if ( rImage[i] > rImage[rMax] ) {
			rMax = i;
		}
	}

	return cv::Vec3b( bMax * 4, gMax * 4, rMax * 4 );
}

double sb::LaneComponent::getColorDistance( const cv::Vec3b& color1, const cv::Vec3b& color2 )
{
	cv::Vec3i diff( static_cast<int>(color1[0]) - static_cast<int>(color2[0]),
	                static_cast<int>(color1[1]) - static_cast<int>(color2[1]),
	                static_cast<int>(color1[2]) - static_cast<int>(color2[2]) );

	double d = std::sqrt( diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2] );

	return d;
}

cv::Vec3f sb::LaneComponent::cvtColorBGRtoLab( const cv::Vec3b& bgr )
{
	cv::Mat3f bgrMat = cv::Mat3f::zeros( 1, 1 );
	bgrMat.at<cv::Vec3f>( 0, 0 ) = (cv::Vec3f( bgr.val[0] / 255.0f, bgr.val[1] / 255.0f, bgr.val[2] / 255.0f ));

	cv::Mat3f lab;
	cvtColor( bgrMat, lab, cv::COLOR_BGR2Lab );

	return lab.at<cv::Vec3f>( cv::Point( 0, 0 ) );
}

double sb::LaneComponent::calculateDeltaE( const cv::Vec3f& bgr1, const cv::Vec3f& bgr2, double kL, double kC, double kH )
{
	cv::Vec3f lab1 = cvtColorBGRtoLab( bgr1 );
	cv::Vec3f lab2 = cvtColorBGRtoLab( bgr2 );

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

#ifdef SB_DEBUG
void sb::LaneComponent::drawLanePart( cv::Mat& image, const sb::LanePart& part )
{
	cv::line( image,
	          part.innerLine.getBottomPoint(), part.innerLine.getTopPoint(), cv::Scalar( 0, 255, 0 ), 2 );
	cv::line( image,
	          part.outerLine.getBottomPoint(), part.outerLine.getTopPoint(), cv::Scalar( 0, 255, 0 ), 2 );
	cv::line( image,
	          part.innerLine.getBottomPoint(), part.outerLine.getBottomPoint(), cv::Scalar( 0, 255, 0 ), 2 );
	cv::line( image,
	          part.innerLine.getTopPoint(), part.outerLine.getTopPoint(), cv::Scalar( 0, 255, 0 ), 2 );
}
#endif // drawLanePart()
