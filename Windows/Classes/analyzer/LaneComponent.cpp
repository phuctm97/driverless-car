#include "LaneComponent.h"
#include "../collector/Collector.h"
#include "Lane.h"

// TODO: argument minLaneWidth, maxLaneWidth
// TODO: trường hợp một section bị nhiễu mất line

const std::vector<sb::LanePartInfo>& sb::LaneComponent::getParts() const { return _parts; }

void sb::LaneComponent::init( int side )
{
	_side = side;

	_minLaneWidth = 10;

	_maxLaneWidth = 40;

	_windowWidth = _maxLaneWidth * 2;

	_windowMove = _maxLaneWidth;
}

int sb::LaneComponent::find( const sb::FrameInfo& frameInfo )
{
	// TODO: kiểm tra màu
	// TODO: nối các lane part tìm lại được

	double minX = 0;
	double maxX = 1.0 * frameInfo.getColorImage().cols;
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
		cv::cvtColor( debugImages[1], debugImages[1], cv::COLOR_GRAY2BGR );
#endif // init images

#ifdef SB_DEBUG
		for ( int i = 0; i < 2; i++ ) {
			cv::rectangle( debugImages[i],
			               cv::Point2d( windowX, frameInfo.getImageSections().front().getImageRect().tl().y ),
			               cv::Point2d( windowX + _windowWidth, frameInfo.getImageSections().front().getImageRect().br().y ),
			               cv::Scalar( 0, 0, 255 ), 1 );
			cv::imshow( "Image " + std::to_string( i ), debugImages[i] );
		}
		cv::waitKey( 100 );
#endif // show and draw window

		std::vector<sb::LanePart> firstLaneParts;
		findFirstLaneParts( frameInfo.getImageSections().front(), frameInfo.getColorImage(), windowX, firstLaneParts );

		for ( const auto& firstLanePart : firstLaneParts ) {
			std::vector<sb::LanePart> fullLaneParts;
			double fullLaneRating;

			findBestFullLaneParts( frameInfo.getImageSections(), frameInfo.getColorImage(), firstLanePart, fullLaneParts, fullLaneRating );

#ifdef SB_DEBUG
			for ( int i = 0; i < 2; i++ ) {
				cv::Mat img = debugImages[i].clone();
				for ( const auto& part : fullLaneParts ) {
					drawLanePart( img, part );
				}
				cv::imshow( "Image " + std::to_string( i ), img );
				cv::waitKey( 500 );
			}
#endif //show completed lane

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
	}

	return 0;
}

int sb::LaneComponent::track( const sb::FrameInfo& frameInfo )
{
	// track individual lane part
	auto it_section = frameInfo.getImageSections().cbegin();
	auto it_part = _parts.cbegin();

	std::vector<std::vector<sb::LanePartInfo>> lanePartsTrackResults;
	lanePartsTrackResults.reserve( frameInfo.getImageSections().size() );

	for ( ; it_section != frameInfo.getImageSections().cend(); ++it_section , ++it_part ) {
		size_t index = std::distance( frameInfo.getImageSections().cbegin(), it_section );

		std::vector<sb::LanePartInfo> laneParts;

		int rc;

		rc = trackIndividualLanePart_PlanA( it_part->part, *it_section, frameInfo.getColorImage(), frameInfo.getEdgesImage(), laneParts );

		if ( rc < 0 ) rc = trackIndividualLanePart_PlanB( it_part->part, *it_section, frameInfo.getColorImage(), frameInfo.getEdgesImage(), laneParts );

		if ( rc < 0 ) rc = trackIndividualLanePart_PlanC( it_part->part, *it_section, frameInfo.getColorImage(), frameInfo.getEdgesImage(), laneParts );

		if ( rc < 0 ) rc = trackIndividualLanePart_PlanD( it_part->part, *it_section, frameInfo.getColorImage(), frameInfo.getEdgesImage(), laneParts );

		lanePartsTrackResults.push_back( laneParts );

		concludeIndividualLanePart( laneParts );

	}

	// combine result

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

			double distanceToWhite = getColorDistance( lanePart.laneColor, cv::Vec3b( 255, 255, 255 ) );

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
		if ( angleDiff1 > MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_ADJACENT_LINES
			&& angleDiff2 > MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_ADJACENT_LINES )
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

			double colorDiff = getColorDistance( lanePart.laneColor, lastestLanePart.laneColor );
			if ( colorDiff > MAX_ACCEPTABLE_DISTANCE_COLOR_BETWEEN_LINE )break;

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

			double colorRating = 10.0 * (MAX_ACCEPTABLE_DISTANCE_COLOR_BETWEEN_LINE - colorDiff)
					/ MAX_ACCEPTABLE_DISTANCE_COLOR_BETWEEN_LINE;

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

#ifdef SB_DEBUG
		{
			for ( int i = 0; i < 2; i++ ) {
				cv::Mat img = debugImages[i].clone();

				for ( const auto& part : parts.first ) {
					drawLanePart( img, part );
				}

				cv::imshow( "Image " + std::to_string( i ), img );
				cv::waitKey( 200 );
			}
		}
#endif //show and sequence of lane parts

		// finish a lane
		if ( parts.first.size() == sections.size() ) {
			parts.second /= sections.size() - 1;

			if ( parts.second > fullLaneRating ) {

#ifdef SB_DEBUG
				for ( int i = 0; i < 2; i++ ) {
					cv::Mat img = debugImages[i].clone();

					std::cout << "Distance to white: ";
					for ( const auto& part : parts.first ) {
						drawLanePart( img, part );
						double distanceToWhite = getColorDistance( part.laneColor, cv::Vec3b( 255, 255, 255 ) );
						std::cout << (int) distanceToWhite << " ";
					}
					std::cout << std::endl;

					std::stringstream stringBuilder;
					stringBuilder << "Rating: " << parts.second;
					cv::putText( img, stringBuilder.str(), cv::Point2d( 30, 30 ), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ) );
					cv::imshow( "Image " + std::to_string( i ), img );
					cv::waitKey( 300 );
				}
#endif //show completed lane

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
			std::pair<std::vector<sb::LanePart>, double> tempParts;

			tempParts.first = std::vector<sb::LanePart>( parts.first.cbegin(), parts.first.cend() );
			tempParts.first.push_back( it_next_part->first );
			tempParts.second = parts.second + it_next_part->second;

			stackParts.push( tempParts );
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

			double posError = it_inner->getBottomPoint().x - it_inner->getBottomPoint().x;
			if ( abs( posError ) > MAX_ACCEPTABLE_POSITION_ERROR_TRACK_LINE ) continue;

			for ( auto it_outer = it_inner + 1; it_outer != section.getImageLines().crend(); ++it_outer ) {

				double width = it_inner->getBottomPoint().x - it_outer->getBottomPoint().x;
				if ( width < _minLaneWidth || width > _maxLaneWidth ) continue;

				double width2nd = it_inner->getTopPoint().x - it_outer->getTopPoint().x;
				if ( width2nd < 0 || width2nd - width > 2 ) continue;

				double widthError = abs( width - previousWidth ) / previousWidth;
				if ( widthError > MAX_ACCEPTABLE_WIDTH_ERROR_TRACK_LINE ) continue;

				// TODO: color error

				// TODO: calculate rating

				sb::LanePartInfo lanePartInfo;

				trackResults.push_back( lanePartInfo );
			}

		}
	}
	else {

		double previousWidth = part.outerLine.getBottomPoint().x - part.innerLine.getBottomPoint().x;

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
				if ( widthError > MAX_ACCEPTABLE_WIDTH_ERROR_TRACK_LINE ) continue;

				// TODO: color error

				// TODO: calculate rating
				sb::LanePartInfo lanePartInfo;

				// TODO: set lane part info
				// TODO: set error state is BOTH_LINE_TRACKED

				trackResults.push_back( lanePartInfo );
			}

		}
	}

	// TODO: return 0 if there're some good result

	// TODO: return -1 if too few results received

	return 0;
}

int sb::LaneComponent::trackIndividualLanePart_PlanB( const sb::LanePart& part,
                                                      const sb::Section& section,
                                                      const cv::Mat& colorImage,
                                                      const cv::Mat& edgesImage,
                                                      std::vector<sb::LanePartInfo>& trackResults )
{
	for ( auto it_inner = section.getImageLines().cbegin();
	      it_inner != section.getImageLines().cend(); ++it_inner ) {

		double angleError = it_inner->getAngle() - part.innerLine.getAngle();
		if ( abs( angleError ) > MAX_ACCEPTABLE_ANGLE_ERROR_TRACK_LINE ) continue;

		double posError = it_inner->getBottomPoint().x - part.innerLine.getBottomPoint().x;
		if ( abs( posError ) > MAX_ACCEPTABLE_POSITION_ERROR_TRACK_LINE ) continue;

		// TODO: color check

		// TODO: calculate rating

		sb::LanePartInfo lanePartInfo;

		// TODO: set lane part info
		// TODO: set error state is OUTER_LINE_LOST

		trackResults.push_back( lanePartInfo );
	}

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

void sb::LaneComponent::concludeIndividualLanePart( const std::vector<sb::LanePartInfo>& trackedLaneParts ) { }

void sb::LaneComponent::getPartColor( const cv::Mat& colorImage, sb::LanePart& part )
{
	cv::Point rectPoints[4] = {
		cv::Point( static_cast<int>(MIN( colorImage.cols - 1, MAX( 0, part.innerLine.getTopPoint().x ) )),
		           static_cast<int>(MIN( colorImage.rows - 1, MAX( 0, part.innerLine.getTopPoint().y ) )) ),
		cv::Point( static_cast<int>(MIN( colorImage.cols - 1, MAX( 0, part.innerLine.getBottomPoint().x ) )),
		           static_cast<int>(MIN( colorImage.rows - 1, MAX( 0,part.innerLine.getBottomPoint().y ) )) ),
		cv::Point( static_cast<int>(MIN( colorImage.cols - 1, MAX( 0, part.outerLine.getBottomPoint().x ) )),
		           static_cast<int>(MIN( colorImage.rows - 1, MAX( 0,part.outerLine.getBottomPoint().y ) )) ) ,
		cv::Point( static_cast<int>(MIN( colorImage.cols - 1, MAX( 0, part.outerLine.getTopPoint().x ) )),
		           static_cast<int>(MIN( colorImage.rows - 1, MAX( 0,part.outerLine.getTopPoint().y ) )) ) };

	part.laneColor = getMainColor( colorImage, rectPoints );
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
