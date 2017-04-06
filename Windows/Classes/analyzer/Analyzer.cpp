#include "Analyzer.h"

// TODO : sửa warp với chế độ mới -> OK
// TODO : cơ chế chuyển tọa độ 2 chiều -> OK
// TODO : sử dụng màu sắc để đánh giá đường
// TODO : đánh giá điểm chi tiết hơn
// TODO : cho Analyzer học về các thuộc tính của đường

int sb::Analyzer::init( const sb::Params& params )
{
	_minLandWidth = params.MIN_LANE_WIDTH;
	_maxLandWidth = params.MAX_LANE_WIDTH;
	_minRoadWidth = params.MIN_ROAD_WIDTH;
	_maxRoadWidth = params.MAX_ROAD_WIDTH;

	// loosen criteras
	// _minLandWidth *= 0.9;
	// _maxLandWidth *= 1.1;

	_windowSize = cv::Size( static_cast<int>(_maxLandWidth) * 2, static_cast<int>(_maxLandWidth) * 2 );
	_windowMove = cv::Point2d( _maxLandWidth, _maxLandWidth );
	_topRightCorner = cv::Point2d( 50, 50 ); //** need to be set
	_lanePartLength = 12;

	_learntLaneWidth = -1;
	_learntRoadWidth = -1;

	_learnLaneWidthLive = LEARNT_LANE_WIDTH_OFF_TO_LIVE;
	_learntRoadWidthLive = LEARNT_ROAD_WIDTH_OFF_TO_LIVE;

#ifdef SB_DEBUG
	// coord converter
	{
		_debugFormatter = sb::Formatter( params.CROP_BOX,
		                                 params.WARP_SRC_QUAD,
		                                 params.WARP_DST_QUAD,
		                                 params.CONVERT_COORD_COEF );
	}
#endif //SB_DEBUG

	return 0;
}

int sb::Analyzer::analyze( const sb::FrameInfo& frameInfo, sb::RoadInfo& roadInfo )
{
	return analyze3( frameInfo, roadInfo );
}

void sb::Analyzer::release() { }

int sb::Analyzer::analyze3( const sb::FrameInfo& frameInfo, sb::RoadInfo& roadInfo )
{
	const size_t num_lines = frameInfo.getRealLineInfos().size();

	sb::Timer timer;
	timer.reset( "analyzer" );

#ifdef SB_DEBUG
	// main debug components
	const cv::Size expand_size( 900, 700 );
	cv::Mat real_image;
	// create real image
	{
		real_image = cv::Mat( frameInfo.getColorImage().rows + expand_size.height,
		                      frameInfo.getColorImage().cols + expand_size.width, CV_8UC3, cv::Scalar( 0, 0, 0 ) );

		cv::line( real_image,
		          _debugFormatter.convertFromCoord( cv::Point2d( 0, 0 ) )
		          + cv::Point2d( expand_size.width / 2, expand_size.height ),
		          _debugFormatter.convertFromCoord( cv::Point2d( 0, 500 ) )
		          + cv::Point2d( expand_size.width / 2, expand_size.height ),
		          cv::Scalar( 255, 255, 255 ), 1 );

		for ( size_t i = 0; i < num_lines; i++ ) {
			const auto& line = frameInfo.getRealLineInfos()[i];

			cv::line( real_image,
			          _debugFormatter.convertFromCoord( line.getStartingPoint() )
			          + cv::Point2d( expand_size.width / 2, expand_size.height ),
			          _debugFormatter.convertFromCoord( line.getEndingPoint() )
			          + cv::Point2d( expand_size.width / 2, expand_size.height ), cv::Scalar( 0, 0, 255 ), 1 );
		}

		// cv::imshow( "Analyzer", real_image );
		// cv::waitKey();
	}
#endif //SB_DEBUG

	// window //** consider use bigger window to find first lane part
	cv::Rect2d window( -_topRightCorner.x, 0, _windowSize.width, _windowSize.height );

	bool has_found_good_road = false;
	sb::Road final_road;
	final_road.rating = 0;

	do {

#ifdef SB_DEBUG
		///// debug /////
		cv::Mat temp_image; {
			temp_image = real_image.clone();
			cv::rectangle( temp_image,
			               _debugFormatter.convertFromCoord( window.tl() )
			               + cv::Point2d( expand_size.width / 2, expand_size.height ),
			               _debugFormatter.convertFromCoord( window.br() )
			               + cv::Point2d( expand_size.width / 2, expand_size.height ), cv::Scalar::all( 255 ) );
		}
		///// debug /////
#endif //SB_DEBUG

		std::vector<sb::LanePart> first_lane_parts;

#ifdef SB_DEBUG
		timer.reset( "findFirstLaneParts" );
		findFirstLaneParts( frameInfo.getRealLineInfos(), window, first_lane_parts, temp_image, expand_size );
		std::cout << "findFirstLaneParts: " << timer.milliseconds( "findFirstLaneParts" ) << "ms." << std::endl;
#else
		findFirstLaneParts( frameInfo.getRealLineInfos(), window, first_lane_parts );
#endif //SB_DEBUG

#ifdef SB_DEBUG
		// show first lane parts
		/*{
			for ( const auto& lane_part: first_lane_parts ) {
				cv::Mat img = temp_image.clone();
				drawLanePart( lane_part, img, expand_size, cv::Scalar( 255, 255, 255 ), 2 );
				cv::imshow( "Analyzer", img );
				cv::waitKey( 200 );
			}
		}*/
#endif //SB_DEBUG

		for ( const auto& first_lane_part : first_lane_parts ) {
			std::vector<sb::Lane> lanes;

#ifdef SB_DEBUG
			timer.reset( "findLanes" );
			findLanes( frameInfo.getRealLineInfos(), first_lane_part, lanes, temp_image, expand_size );
			std::cout << "findLanes: " << timer.milliseconds( "findLanes" ) << "ms." << std::endl;
#else
			findLanes( frameInfo.getRealLineInfos(), first_lane_part, lanes );
#endif

			for ( size_t lane_index = 0; lane_index < lanes.size(); lane_index++ ) {
				const sb::Lane& lane = lanes[lane_index];

				std::vector<sb::Road> roads;

#ifdef SB_DEBUG
				// show lane parts
				cv::Mat img = temp_image.clone(); {
					for ( const auto& lane_part : lane.parts ) {
						drawLanePart( lane_part, img, expand_size, cv::Scalar( 0, 255, 255 ), 2 );
					}
				}

				// find road
				timer.reset( "findRoads" );
				findRoads( frameInfo.getRealLineInfos(), lane, roads, img, expand_size );
				std::cout << "findRoads: " << timer.milliseconds( "findRoads" ) << "ms." << std::endl;

				// show road and information
				/*for ( const auto& road : roads ) {
					cv::Mat tmp_img = img.clone();
					for ( size_t knot_index = 0; knot_index < road.knots.size() - 1; knot_index++ ) {
						cv::line( tmp_img,
						          _debugFormatter.convertFromCoord( road.knots[knot_index].first )
						          + cv::Point2d( expand_size.width / 2, expand_size.height ),
						          _debugFormatter.convertFromCoord( road.knots[knot_index + 1].first )
						          + cv::Point2d( expand_size.width / 2, expand_size.height ),
						          cv::Scalar( 255, 255, 255 ), 4 );
						cv::line( tmp_img,
						          _debugFormatter.convertFromCoord( road.knots[knot_index].second )
						          + cv::Point2d( expand_size.width / 2, expand_size.height ),
						          _debugFormatter.convertFromCoord( road.knots[knot_index + 1].second )
						          + cv::Point2d( expand_size.width / 2, expand_size.height ),
						          cv::Scalar( 255, 255, 255 ), 4 );
					}
					std::stringstream stringBuilder;

					stringBuilder << "Lane Rating: " << lane.rating;
					cv::putText( tmp_img, stringBuilder.str(), cv::Point2d( 50, 15 ), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ) );
					stringBuilder.str( "" );

					stringBuilder << "Road Rating: " << road.rating;
					cv::putText( tmp_img, stringBuilder.str(), cv::Point2d( 50, 35 ), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ) );
					stringBuilder.str( "" );

					cv::imshow( "Analyzer", tmp_img );
					cv::waitKey();
				}*/
#else
				findRoads( frameInfo.getRealLineInfos(), lane, roads );
#endif //SB_DEBUG

				if ( roads.front().rating > final_road.rating ) {
					final_road = roads.front();
				}

				//**consider another function to determine good information
				if ( final_road.rating >= GOOD_ROAD_RATING || lane.rating >= GOOD_LANE_RATING ) {
					has_found_good_road = true;
				}

				if ( has_found_good_road || timer.milliseconds( "analyzer" ) > ANALYZER_TIMEOUT ) break;
			}

			if ( has_found_good_road || timer.milliseconds( "analyzer" ) > ANALYZER_TIMEOUT ) break;
		}

		// move window around frame to find lane candidate until it meets some requirements
		if ( moveMainWindow( window ) < 0 ) break;

		if ( has_found_good_road || timer.milliseconds( "analyzer" ) > ANALYZER_TIMEOUT ) break;

	} while ( true );

#ifdef SB_DEBUG
	std::cout << "analyzer" << timer.milliseconds( "analyzer" ) << "ms." << std::endl;

	// show final road
	{
		cv::Mat img = real_image.clone();

		for ( int knot_index = 0; knot_index < static_cast<int>(final_road.knots.size()) - 1; knot_index++ ) {
			cv::line( img,
			          _debugFormatter.convertFromCoord( final_road.knots[knot_index].first )
			          + cv::Point2d( expand_size.width / 2, expand_size.height ),
			          _debugFormatter.convertFromCoord( final_road.knots[knot_index + 1].first )
			          + cv::Point2d( expand_size.width / 2, expand_size.height ),
			          cv::Scalar( 255, 255, 255 ), 4 );
			cv::line( img,
			          _debugFormatter.convertFromCoord( final_road.knots[knot_index].second )
			          + cv::Point2d( expand_size.width / 2, expand_size.height ),
			          _debugFormatter.convertFromCoord( final_road.knots[knot_index + 1].second )
			          + cv::Point2d( expand_size.width / 2, expand_size.height ),
			          cv::Scalar( 255, 255, 255 ), 4 );
		}
		std::stringstream stringBuilder;

		stringBuilder << "Road Rating: " << final_road.rating;
		cv::putText( img, stringBuilder.str(), cv::Point2d( 50, 35 ), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ) );
		stringBuilder.str( "" );

		cv::imshow( "Analyzer", img );
		cv::waitKey();
	}
#endif //SB_DEBUG

	learnNewProperties( final_road );

	calculateRoadTarget( final_road, roadInfo );

	return 0;
}

#ifdef SB_DEBUG
void sb::Analyzer::findFirstLaneParts( const std::vector<sb::LineInfo>& line_lists,
                                       const cv::Rect2d& window,
                                       std::vector<sb::LanePart>& first_lane_parts,
                                       const cv::Mat& image,
                                       const cv::Size& expand_size ) const
#else
void sb::Analyzer::findFirstLaneParts( const std::vector<sb::LineInfo>& line_lists,
                                       const cv::Rect2d& window,
                                       std::vector<sb::LanePart>& first_lane_parts ) const
#endif //SB_DEBUG
{
	first_lane_parts.clear();

	std::vector<sb::LineInfo> lines_intersect_window;
	findLinesIntersectRectangle( line_lists, window, lines_intersect_window );

	const size_t n_lines = lines_intersect_window.size();

	for ( size_t first_index = 0; first_index < n_lines; first_index++ ) {
		const auto& first_line = lines_intersect_window[first_index];

		for ( size_t second_index = first_index + 1; second_index < n_lines; second_index++ ) {
			const auto& second_line = lines_intersect_window[second_index];

			// lane info
			cv::Point2d pos;
			double width, angle;

			// check angle
			double angle_diff = abs( first_line.getAngle() - second_line.getAngle() );
			if ( angle_diff > MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_LINES ) continue;

			angle = (first_line.getAngle() + second_line.getAngle()) / 2;

			// calculate two intersect points
			cv::Point2d first_point, second_point;
			first_point = first_line.getStartingPoint();

			sb::Line horizon( angle + 90, first_point );
			sb::Line::findIntersection( horizon, second_line.getLine(), second_point );

			// check position
			if ( horizon.f( second_line.getStartingPoint() ) * horizon.f( second_line.getEndingPoint() ) > 0 ) {
				double verticalDistance;
				cv::Point2d verticalDiff;
				verticalDiff = second_line.getStartingPoint() - second_point;
				verticalDistance = std::sqrt( verticalDiff.x * verticalDiff.x + verticalDiff.y * verticalDiff.y );
				verticalDiff = second_line.getEndingPoint() - second_point;
				verticalDistance = MIN( verticalDistance, std::sqrt( verticalDiff.x*verticalDiff.x + verticalDiff.y*verticalDiff.y ) );
				if ( verticalDistance > MAX_ACCEPTABLE_VERTICAL_DIFF_BETWEEN_LINES ) continue;
			}

			// check width
			cv::Point2d posDiff = first_point - second_point;
			width = std::sqrt( posDiff.x * posDiff.x + posDiff.y * posDiff.y );

			if ( width < _minLandWidth || width > _maxLandWidth ) continue;

			// select lane origin
			if ( first_point.x < second_point.x )
				pos = first_point;
			else if ( second_point.x < first_point.x )
				pos = second_point;
			else pos = first_point.y < second_point.y ? first_point : second_point;

			sb::LanePart lane_part( pos, angle, width, _lanePartLength );

			// there have already been a lane part similar to this
			if ( std::find( first_lane_parts.cbegin(), first_lane_parts.cend(), lane_part )
				!= first_lane_parts.cend() )
				continue;

			first_lane_parts.push_back( lane_part );
		}
	}
}

#ifdef SB_DEBUG
void sb::Analyzer::findLanes( const std::vector<sb::LineInfo>& lines_list,
                              const sb::LanePart& first_lane_part,
                              std::vector<sb::Lane>& lanes,
                              const cv::Mat& image,
                              const cv::Size& expand_size ) const
#else
void sb::Analyzer::findLanes( const std::vector<sb::LineInfo>& lines_list,
                              const sb::LanePart& first_lane_part,
                              std::vector<sb::Lane>& lanes ) const
#endif //SB_DEBUG
{
	lanes.clear();

	int hops_to_live = NUM_LANE_PARTS;

	std::stack<sb::Lane> stack_lanes;

	// init lane
	{
		sb::Lane temp_lane;
		temp_lane.rating = 10;
		temp_lane.parts.push_back( first_lane_part );

		stack_lanes.push( temp_lane );
	}

	while ( !stack_lanes.empty() ) {

		// pop a bufferred scenario, find and push it new lane part
		sb::Lane lane = stack_lanes.top();
		stack_lanes.pop();

		// lastest lane part
		sb::LanePart lastest_lane_part = lane.parts.back();

		// move window to cover center point
		cv::Point2d window_origin = (lastest_lane_part.vertices[1] + lastest_lane_part.vertices[2]) * 0.5;

		//** consider use smaller window to find next lane part
		cv::Rect2d window( window_origin.x - _windowSize.width / 2,
		                   window_origin.y - _windowSize.height / 2, _windowSize.width, _windowSize.height );

#ifdef SB_DEBUG
		// show window and sequence of lane parts
		/*{
			cv::Mat temp_image = image.clone();

			cv::rectangle( temp_image,
			               _debugFormatter.convertFromCoord( window.tl() )
			               + cv::Point2d( expand_size.width / 2, expand_size.height ),
			               _debugFormatter.convertFromCoord( window.br() )
			               + cv::Point2d( expand_size.width / 2, expand_size.height ),
			               cv::Scalar( 0, 255, 255 ), 1 );

			for ( const auto& lane_part : lane.parts ) {
				drawLanePart( lane_part, temp_image, expand_size, cv::Scalar::all( 255 ), 2 );
			}

			cv::imshow( "Analyzer", temp_image );
			cv::waitKey( 200 );
		}*/
#endif //SB_DEBUG

		// finish a lane
		if ( lane.parts.size() == hops_to_live ) {
			lane.rating /= hops_to_live;

			if ( lane.rating > BAD_LINE_RATING ) {
				lanes.push_back( lane );

#ifdef SB_DEBUG
				/*{
					cv::Mat img = image.clone();
					for ( const auto& lane_part : lane.parts ) {
						drawLanePart( lane_part, img, expand_size, cv::Scalar( 0, 255, 255 ), 2 );
					}
					std::stringstream stringBuilder;
					stringBuilder << "Rating: " << lane.rating;
					cv::putText( img, stringBuilder.str(), cv::Point2d( 30, 30 ), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ) );
					cv::imshow( "Analyzer", img );
					cv::waitKey();
				}*/
#endif

				if ( lane.rating >= GOOD_LANE_RATING ) break;
			}
			continue;
		}

		// find lines in window
		std::vector<sb::LineInfo> lines_intersect_window;
		findLinesIntersectRectangle( lines_list, window, lines_intersect_window );

		// filter unsuitable lines 
		for ( size_t i = 0; i < lines_intersect_window.size(); ) {
			const auto& line = lines_intersect_window[i];

			double angle_diff = abs( line.getAngle() - lastest_lane_part.angle );

			//** allow argument to be set
			if ( angle_diff > MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_LANE_PARTS ) {
				lines_intersect_window.erase( lines_intersect_window.begin() + i );
				continue;
			}
			i++;
		}

		// find next lane parts
		std::vector<sb::LanePart> next_lane_parts;
		std::vector<double> next_lane_part_ratings;

#ifdef SB_DEBUG
		findNextLaneParts( lines_intersect_window, lastest_lane_part, next_lane_parts, next_lane_part_ratings, image, expand_size );
#else
		findNextLaneParts( lines_intersect_window, lastest_lane_part, next_lane_parts, next_lane_part_ratings );
#endif //SB_DEBUG

		// if none of next part found, copy old lane
		if ( next_lane_parts.empty() ) {
			next_lane_parts.push_back( sb::LanePart( lastest_lane_part.vertices[1],
			                                         lastest_lane_part.angle,
			                                         lastest_lane_part.width,
			                                         _lanePartLength ) );
			next_lane_part_ratings.push_back( 0 );
		}

		// push new lanes in to stack
		for ( size_t i = 0; i < next_lane_parts.size(); i++ ) {
			sb::Lane temp_lane;
			temp_lane.parts = std::vector<sb::LanePart>( lane.parts.cbegin(), lane.parts.cend() );
			temp_lane.parts.push_back( next_lane_parts[i] );
			temp_lane.rating = lane.rating + next_lane_part_ratings[i];

			stack_lanes.push( temp_lane );
		}
	}

	std::sort( lanes.begin(), lanes.end(),
	           []( const sb::Lane& l1, const sb::Lane& l2 )-> bool { return l1.rating > l2.rating; } );
}

#ifdef SB_DEBUG
void sb::Analyzer::findNextLaneParts( const std::vector<sb::LineInfo>& lines_list,
                                      const sb::LanePart& lastest_lane_part,
                                      std::vector<sb::LanePart>& next_lane_parts,
                                      std::vector<double>& next_lane_part_ratings,
                                      const cv::Mat& image,
                                      const cv::Size& expand_size ) const
#else
void sb::Analyzer::findNextLaneParts( const std::vector<sb::LineInfo>& lines_list,
                                      const sb::LanePart& lastest_lane_part,
                                      std::vector<sb::LanePart>& next_lane_parts,
                                      std::vector<double>& next_lane_part_ratings ) const
#endif //SB_DEBUG
{
	next_lane_parts.clear();
	next_lane_part_ratings.clear();

	const size_t n_lines = lines_list.size();

	for ( size_t first_index = 0; first_index < n_lines; first_index++ ) {
		const auto& first_line = lines_list[first_index];

		for ( size_t second_index = first_index + 1; second_index < n_lines; second_index++ ) {
			const auto& second_line = lines_list[second_index];

			/// 1) check and calculate lane info

			// lane info
			cv::Point2d pos;
			double width, angle;

			// check angle
			double angle_diff = abs( first_line.getAngle() - second_line.getAngle() );
			if ( angle_diff > MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_LINES ) continue;

			angle = (first_line.getAngle() + second_line.getAngle()) / 2;

			double angle_diff_with_lastest = abs( angle - lastest_lane_part.angle );
			if ( angle_diff_with_lastest > MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_LANE_PARTS ) continue;

			// calculate two intersect points
			cv::Point2d first_point;

			sb::Line lastest_horizon( lastest_lane_part.vertices[1], lastest_lane_part.vertices[2] );
			sb::Line::findIntersection( lastest_horizon, first_line.getLine(), first_point );

			cv::Point2d second_point;

			sb::Line horizon( angle + 90, first_point );
			sb::Line::findIntersection( horizon, second_line.getLine(), second_point );

			// check position
			if ( horizon.f( second_line.getStartingPoint() ) * horizon.f( second_line.getEndingPoint() ) > 0 ) {
				double verticalDistance;
				cv::Point2d verticalDiff;
				verticalDiff = second_line.getStartingPoint() - second_point;
				verticalDistance = std::sqrt( verticalDiff.x * verticalDiff.x + verticalDiff.y * verticalDiff.y );
				verticalDiff = second_line.getEndingPoint() - second_point;
				verticalDistance = MIN( verticalDistance, std::sqrt( verticalDiff.x*verticalDiff.x + verticalDiff.y*verticalDiff.y ) );
				if ( verticalDistance > MAX_ACCEPTABLE_VERTICAL_DIFF_BETWEEN_LINES ) continue;
			}

			// check width
			cv::Point2d pos_diff = first_point - second_point;
			width = std::sqrt( pos_diff.x * pos_diff.x + pos_diff.y * pos_diff.y );

			if ( width < _minLandWidth || width > _maxLandWidth ) continue;

			double width_diff_with_lastest = abs( width - lastest_lane_part.width );
			if ( width_diff_with_lastest > MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_LANE_PARTS ) continue;

			// select lane origin
			if ( first_point.x < second_point.x )
				pos = first_point;
			else if ( second_point.x < first_point.x )
				pos = second_point;
			else pos = first_point.y < second_point.y ? first_point : second_point;

			// check position diff
			cv::Point2d pos_on_lastest_horizon;
			sb::Line::findIntersection( lastest_horizon, sb::Line( angle, pos ), pos_on_lastest_horizon );

			cv::Point2d pos_diff_with_lastest;
			pos_diff_with_lastest = pos_on_lastest_horizon - lastest_lane_part.vertices[1];

			double pos_distance_with_lastest = std::sqrt( pos_diff_with_lastest.x * pos_diff_with_lastest.x +
			                                             pos_diff_with_lastest.y * pos_diff_with_lastest.y );

			//** argument need to be set
			if ( pos_distance_with_lastest > MAX_ACCEPTABLE_DISTANCE_BETWEEN_LANE_PARTS ) continue;

			/// 2) final check and calculate rating
			sb::LanePart lane_part( pos, angle, width, _lanePartLength );

			if ( std::find( next_lane_parts.cbegin(), next_lane_parts.cend(), lane_part )
				!= next_lane_parts.cend() )
				continue;

			//** consider new function
			double width_rating = 10 - (10.0 / MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_LANE_PARTS) * width_diff_with_lastest;
			double pos_rating = 10 - (10.0 / MAX_ACCEPTABLE_DISTANCE_BETWEEN_LANE_PARTS) * pos_distance_with_lastest;

			double rating = 0.65 * width_rating + 0.35 * pos_rating;

			/*if ( _learntLaneWidth > 0 ) {
				double width_diff_with_learnt = abs( width - _learntLaneWidth );
				double learnt_width_rating = 0;
				if ( width_diff_with_learnt <= MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_LANE_PARTS ) {
					learnt_width_rating = 10.0 * (MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_LANE_PARTS - width_diff_with_learnt)
							/ MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_LANE_PARTS;
				}
				rating = 0.7 * rating + 0.3 * learnt_width_rating;
			}*/

			next_lane_parts.push_back( lane_part );
			next_lane_part_ratings.push_back( rating );
		}
	}
}

#ifdef SB_DEBUG
void sb::Analyzer::findRoads( const std::vector<sb::LineInfo>& lines_list,
                              const sb::Lane& lane,
                              std::vector<sb::Road>& roads,
                              const cv::Mat& image,
                              const cv::Size& expand_size ) const
#else
void sb::Analyzer::findRoads( const std::vector<sb::LineInfo>& lines_list,
                              const sb::Lane& lane,
                              std::vector<sb::Road>& roads ) const
#endif //SBDEBUG

{
	roads.clear();

	// find list of possible side and road width
	std::vector<std::pair<int, double>> scenarios;

#ifdef SB_DEBUG
	findPossibleSideAndRoadWidths( lines_list, lane, scenarios, image, expand_size );
#else
	findPossibleSideAndRoadWidths( lines_list, lane, scenarios );
#endif //SB_DEBUG

	if ( scenarios.empty() ) {
		std::pair<int, double> scenario;

		cv::Point2d pos = lane.parts[0].vertices[0];
		if ( pos.x < 0 )scenario.first = -1;
		else scenario.first = 1;

		if ( _learntRoadWidth > 0 )
			scenario.second = _learntRoadWidth;
		else
			scenario.second = _maxLandWidth / 2;

		scenarios.push_back( scenario );
	}

	for ( const auto& scenario : scenarios ) {
		sb::Road road;
		road.rating = 0;

		road.lane_width = 0;

		// road width
		road.road_width = scenario.second;

		// assign knots
		road.knots.assign( lane.parts.size(), std::pair<cv::Point2d, cv::Point2d>() );

		for ( size_t part_index = 0; part_index < lane.parts.size(); part_index++ ) {
			const auto& lane_part = lane.parts[part_index];

			sb::Line horizon( lane_part.vertices[1], lane_part.vertices[2] );
			sb::Line vertical( lane_part.vertices[0], lane_part.vertices[1] );
			double right_value = vertical.f( lane_part.vertices[2] ) > 0 ? 1 : -1;

			// calculate horizon vec
			cv::Point2d horizon_vec = lane_part.vertices[2] - lane_part.vertices[1];
			double horizon_vec_length = std::sqrt( horizon_vec.x * horizon_vec.x + horizon_vec.y * horizon_vec.y );
			horizon_vec = cv::Point2d( horizon_vec.x / horizon_vec_length * road.road_width,
			                           horizon_vec.y / horizon_vec_length * road.road_width );

			// lane width
			road.lane_width += lane_part.width;

			// knots
			if ( scenario.first > 0 ) {
				road.knots[part_index].first = lane_part.vertices[2];
				road.knots[part_index].second = lane_part.vertices[2] + horizon_vec;
				road.main_side = -1;
			}
			else {
				road.knots[part_index].second = lane_part.vertices[1];
				road.knots[part_index].first = lane_part.vertices[1] - horizon_vec;
				road.main_side = 1;
			}

			// rating
			double part_rating = 0;

			for ( const auto& line : lines_list ) {
				// check angle
				double angle_diff = abs( line.getAngle() - lane_part.angle );

				if ( angle_diff > MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_TWO_LANES ) continue;

				// check position
				double distance_to_horizon = horizon.getDistance( line.getMiddlePoint() );

				if ( distance_to_horizon > _windowSize.height ) continue;

				// check road width
				cv::Point2d knot;
				sb::Line::findIntersection( horizon, line.getLine(), knot );

				cv::Point2d pos_diff = knot - lane_part.vertices[1];
				double road_width = std::sqrt( pos_diff.x * pos_diff.x + pos_diff.y * pos_diff.y );

				double road_width_diff = abs( road_width - scenario.second );

				if ( road_width_diff > MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_TWO_LANES ) continue;

				// check side
				int side = vertical.f( knot ) * right_value > 0 ? 1 : -1;
				if ( side != scenario.first ) continue;

				// calculate rating
				double angle_rating = 10 * (MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_TWO_LANES - angle_diff)
						/ MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_TWO_LANES;

				double road_width_rating = 10 * (MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_TWO_LANES - road_width_diff)
						/ MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_TWO_LANES;

				part_rating = MAX(part_rating, (0.5 * angle_rating + 0.5 * road_width_rating));
			}

			road.rating += part_rating;
		}

		road.rating /= lane.parts.size();

		road.lane_width /= lane.parts.size();

		road.rating = 0.4 * lane.rating + 0.6 * road.rating;

		road.main_lane_rating = lane.rating;

		/*if ( _learntRoadWidth > 0 ) {
			double width_diff_with_learnt = abs( road.road_width - _learntRoadWidth );
			double learnt_rating = 0;
			if ( width_diff_with_learnt <= MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_TWO_LANES ) {
				learnt_rating = 10.0 * (MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_TWO_LANES - width_diff_with_learnt)
						/ MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_TWO_LANES;
			}
			road.rating = 0.7 * road.rating + 0.3 * learnt_rating;
		}*/

#ifdef SB_DEBUG
		// show road and information
		/*{
			cv::Mat img = image.clone();
			for ( size_t knot_index = 0; knot_index < road.knots.size() - 1; knot_index++ ) {
				cv::line( img,
				          _debugFormatter.convertFromCoord( road.knots[knot_index].first )
				          + cv::Point2d( expand_size.width / 2, expand_size.height ),
				          _debugFormatter.convertFromCoord( road.knots[knot_index + 1].first )
				          + cv::Point2d( expand_size.width / 2, expand_size.height ),
				          cv::Scalar( 255, 255, 255 ), 4 );
				cv::line( img,
				          _debugFormatter.convertFromCoord( road.knots[knot_index].second )
				          + cv::Point2d( expand_size.width / 2, expand_size.height ),
				          _debugFormatter.convertFromCoord( road.knots[knot_index + 1].second )
				          + cv::Point2d( expand_size.width / 2, expand_size.height ),
				          cv::Scalar( 255, 255, 255 ), 4 );
			}
			std::stringstream stringBuilder;

			stringBuilder << "Lane Rating: " << lane.rating;
			cv::putText( img, stringBuilder.str(), cv::Point2d( 50, 15 ), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ) );
			stringBuilder.str( "" );

			stringBuilder << "Road Rating: " << road.rating;
			cv::putText( img, stringBuilder.str(), cv::Point2d( 50, 35 ), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ) );
			stringBuilder.str( "" );

			cv::imshow( "Analyzer", img );
			cv::waitKey();
		}*/
#endif //SB_DEBUG

		roads.push_back( road );
	}

	std::sort( roads.begin(), roads.end(),
	           []( const sb::Road& r1, const sb::Road& r2 )-> bool { return r1.rating > r2.rating; } );
}

#ifdef SB_DEBUG
void sb::Analyzer::findPossibleSideAndRoadWidths( const std::vector<sb::LineInfo>& lines_list,
                                                  const sb::Lane& lane,
                                                  std::vector<std::pair<int, double>>& scenarios,
                                                  const cv::Mat& image,
                                                  const cv::Size& expand_size ) const
#else
void sb::Analyzer::findPossibleSideAndRoadWidths( const std::vector<sb::LineInfo>& lines_list,
                                                  const sb::Lane& lane,
                                                  std::vector<std::pair<int, double>>& scenarios ) const
#endif //SB_DEBUG
{
	scenarios.clear();

	for ( const auto& lane_part: lane.parts ) {

		sb::Line horizon( lane_part.vertices[1], lane_part.vertices[2] );
		sb::Line vertical( lane_part.vertices[0], lane_part.vertices[1] );
		double right_value = vertical.f( lane_part.vertices[2] ) > 0 ? 1 : -1; // for calculate side

		// find lines contribute road
		for ( const auto& line: lines_list ) {

			// check angle
			double angle_diff = abs( line.getAngle() - lane_part.angle );

			if ( angle_diff > MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_TWO_LANES ) continue;

			// check position
			double distance_to_horizon = horizon.getDistance( line.getMiddlePoint() );

			if ( distance_to_horizon > _windowSize.height ) continue;

			// find knot to calculate road width
			cv::Point2d knot;
			sb::Line::findIntersection( horizon, line.getLine(), knot );

			// calculate side
			int side = vertical.f( knot ) * right_value > 0 ? 1 : -1;

			// check road width
			cv::Point2d pos_diff = knot - (side < 0 ? lane_part.vertices[1] : lane_part.vertices[2]);
			double road_width = std::sqrt( pos_diff.x * pos_diff.x + pos_diff.y * pos_diff.y );

			if ( road_width < _minRoadWidth || road_width > _maxRoadWidth ) continue;

			// check if there've already been line similar to this
			bool already_exist = false;
			for ( const auto& scenario: scenarios ) {
				if ( scenario.first == side && abs( scenario.second - road_width ) < 5 ) {
					already_exist = true;
					break;
				}
			}

			if ( already_exist ) continue;

			scenarios.push_back( std::pair<int, double>( side, road_width ) );
		}

	}
}

void sb::Analyzer::calculateRoadTarget( const sb::Road& final_road, sb::RoadInfo& roadInfo ) const
{
	if ( final_road.knots.empty() ) {
		roadInfo.setTarget( cv::Point2d( 0, 10 ) );
		return;
	}

	double rate = MAX( final_road.rating / GOOD_ROAD_RATING, final_road.main_lane_rating / GOOD_LANE_RATING );
	rate = MIN( 1, rate );

	int index = static_cast<int>((NUM_LANE_PARTS - 1) * rate);

	cv::Point2d horizon_vec = final_road.knots[index].second - final_road.knots[index].first;
	double horizon_vec_length = std::sqrt( horizon_vec.x * horizon_vec.x + horizon_vec.y * horizon_vec.y );

	horizon_vec = cv::Point2d( horizon_vec.x / horizon_vec_length * final_road.road_width * 0.4,
	                           horizon_vec.y / horizon_vec_length * final_road.road_width * 0.4 );

	cv::Point2d target;
	if ( final_road.main_side < 0 ) {
		target = final_road.knots[index].first + horizon_vec;
	}
	else {
		target = final_road.knots[index].second - horizon_vec;
	}

	roadInfo.setTarget( target );
}

void sb::Analyzer::learnNewProperties( const sb::Road& final_road )
{
	if ( final_road.main_lane_rating > GOOD_LANE_RATING ) {
		_learntLaneWidth = 0.5 * _learntLaneWidth + 0.5 * final_road.lane_width;
		_learnLaneWidthLive = LEARNT_LANE_WIDTH_OFF_TO_LIVE;
	}
	else {
		_learnLaneWidthLive--;
	}

	if ( final_road.rating > GOOD_ROAD_RATING ) {
		_learntRoadWidth = 0.5 * _learntRoadWidth + 0.5 * final_road.road_width;
		_learntRoadWidthLive = LEARNT_ROAD_WIDTH_OFF_TO_LIVE;
	}
	else {
		_learntRoadWidthLive--;
	}

	// lives out
	if ( _learnLaneWidthLive <= 0 ) {
		_learntLaneWidth = -1;
	}
	if ( _learntRoadWidthLive <= 0 ) {
		_learntRoadWidth = -1;
	}
}

int sb::Analyzer::moveMainWindow( cv::Rect2d& window ) const
{
	window.x += _windowMove.x;
	if ( window.x + window.width > _topRightCorner.x ) {
		window.x = -_topRightCorner.x;
		window.y += _windowMove.y;
		if ( window.y > _topRightCorner.y ) return -1;
	}

	return 0;
}

int sb::Analyzer::drawLanePart( const sb::LanePart& lane_part, cv::Mat& image,
                                const cv::Size& expand_size,
                                const cv::Scalar& color,
                                int line_width ) const
{
	for ( int v = 0; v < 4; v++ ) {
		cv::line( image,
		          _debugFormatter.convertFromCoord( lane_part.vertices[v] )
		          + cv::Point2d( expand_size.width / 2, expand_size.height ),
		          _debugFormatter.convertFromCoord( lane_part.vertices[(v + 1) % 4] )
		          + cv::Point2d( expand_size.width / 2, expand_size.height ), color, line_width );
	}
	cv::circle( image,
	            _debugFormatter.convertFromCoord( lane_part.position )
	            + cv::Point2d( expand_size.width / 2, expand_size.height ), 5, cv::Scalar( 255, 255, 255 ), 2 );

	return 0;
}

int sb::Analyzer::findLinesIntersectRectangle( const std::vector<sb::LineInfo>& inputLines,
                                               const cv::Rect2d& window,
                                               std::vector<sb::LineInfo>& outputLines ) const
{
	outputLines.clear();

	// find lines inside current window
	for ( size_t line_index = 0; line_index < inputLines.size(); line_index++ ) {
		const sb::LineInfo& line = inputLines[line_index];
		if ( !segmentIntersectRectangle( line.getStartingPoint(), line.getEndingPoint(), window ) ) continue;

		outputLines.push_back( line );
	}

	return 0;
}

bool sb::Analyzer::segmentIntersectRectangle( const cv::Point2d& p1,
                                              const cv::Point2d& p2,
                                              const cv::Rect2d& rect ) const
{
	// Find min and max X for the segment

	double minX = p1.x;
	double maxX = p2.x;

	if ( p1.x > p2.x ) {
		minX = p2.x;
		maxX = p1.x;
	}

	// Find the intersection of the segment's and rectangle's x-projections

	if ( maxX > rect.x + rect.width ) {
		maxX = rect.x + rect.width;
	}

	if ( minX < rect.x ) {
		minX = rect.x;
	}

	if ( minX > maxX ) // If their projections do not intersect return false
	{
		return false;
	}

	// Find corresponding min and max Y for min and max X we found before

	double minY = p1.y;
	double maxY = p2.y;

	double dx = p2.x - p1.x;

	if ( abs( dx ) > 0.0000001 ) {
		double a = (p2.y - p1.y) / dx;
		double b = p1.y - a * p1.x;
		minY = a * minX + b;
		maxY = a * maxX + b;
	}

	if ( minY > maxY ) {
		double tmp = maxY;
		maxY = minY;
		minY = tmp;
	}

	// Find the intersection of the segment's and rectangle's y-projections

	if ( maxY > rect.y + rect.height ) {
		maxY = rect.y + rect.height;
	}

	if ( minY < rect.y ) {
		minY = rect.y;
	}

	if ( minY > maxY ) // If Y-projections do not intersect return false
	{
		return false;
	}

	return true;
}
