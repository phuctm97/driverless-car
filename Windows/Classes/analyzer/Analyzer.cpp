#include "Analyzer.h"
#include "../collector/RawContent.h"

int sb::Analyzer::init( const sb::Params& params )
{
	_minLandWidth = params.MIN_LANE_WIDTH;
	_maxLandWidth = params.MAX_LANE_WIDTH;
	_minRoadWidth = params.MIN_ROAD_WIDTH;
	_maxRoadWidth = params.MAX_ROAD_WIDTH;

	// loosen criteras
	_minLandWidth *= 0.9;
	_maxLandWidth *= 1.1;

	_windowSize = cv::Size( static_cast<int>(_maxLandWidth) * 2, static_cast<int>(_maxLandWidth) * 2 );
	_windowMove = cv::Point2d( _maxLandWidth, _maxLandWidth );
	_topRightCorner = cv::Point2d( 100, 100 ); //** need to be set
	_lanePartLength = _maxLandWidth;

	// debug //
	cv::Point cropPosition;
	cropPosition.x = (params.COLOR_FRAME_SIZE.width - params.CROPPED_FRAME_SIZE.width) / 2;
	cropPosition.y = params.COLOR_FRAME_SIZE.height - params.CROPPED_FRAME_SIZE.height;

	_debugFormatter = sb::Formatter( cv::Rect( cropPosition.x, cropPosition.y,
	                                           params.CROPPED_FRAME_SIZE.width, params.CROPPED_FRAME_SIZE.height ),
	                                 params.WARP_SRC_QUAD,
	                                 params.WARP_DST_QUAD,
	                                 params.CONVERT_COORD_COEF );
	return 0;
}

int sb::Analyzer::analyze( const sb::FrameInfo& frameInfo, sb::RoadInfo& roadInfo ) const
{
	return analyze3( frameInfo, roadInfo );
}

void sb::Analyzer::release() { }


int sb::Analyzer::find_first_lane_parts( const std::vector<sb::LineInfo>& lines,
                                         std::vector<sb::LanePart>& lane_parts ) const
{
	const size_t n_lines = lines.size();

	for ( size_t first_index = 0; first_index < n_lines; first_index++ ) {
		const auto& first_line = lines[first_index];

		for ( size_t second_index = first_index + 1; second_index < n_lines; second_index++ ) {
			const auto& second_line = lines[second_index];

			/// 1) check and calculate lane info

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

			/// 2) add to good couples
			sb::LanePart lane_part( pos, angle, width, _lanePartLength );

			if ( std::find( lane_parts.cbegin(), lane_parts.cend(), lane_part )
				!= lane_parts.cend() )
				continue;

			lane_parts.push_back( lane_part );
		}
	}

	return 0;
}

int sb::Analyzer::find_next_lane_parts( const std::vector<sb::LineInfo>& lines,
                                        const sb::LanePart& lastest_lane_part,
                                        std::vector<sb::LanePart>& lane_parts,
                                        std::vector<double>& lane_part_ratings ) const
{
	const size_t n_lines = lines.size();

	for ( size_t first_index = 0; first_index < n_lines; first_index++ ) {
		const auto& first_line = lines[first_index];

		for ( size_t second_index = first_index + 1; second_index < n_lines; second_index++ ) {
			const auto& second_line = lines[second_index];

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

			if ( std::find( lane_parts.cbegin(), lane_parts.cend(), lane_part )
				!= lane_parts.cend() )
				continue;

			//** consider new function
			double width_rating = 10 - (10.0 / MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_LANE_PARTS) * width_diff_with_lastest;
			double pos_rating = 10 - (10.0 / MAX_ACCEPTABLE_DISTANCE_BETWEEN_LANE_PARTS) * pos_distance_with_lastest;

			double rating = 0.65 * width_rating + 0.35 * pos_rating;

			lane_parts.push_back( lane_part );
			lane_part_ratings.push_back( rating );
		}
	}

	return 0;
}

int sb::Analyzer::calculate_full_lane( const sb::LanePart& first_lane_part,
                                       const std::vector<sb::LineInfo>& full_lines_list,
                                       sb::Lane& output_lane,
                                       const cv::Mat& image,
                                       const cv::Size& expand_size ) const
{
	int hops_to_live = 7; //** need to be set

	std::vector<sb::Lane> lanes_list;
	std::vector<double> lane_ratings_list;

	std::stack<sb::Lane> stack_lanes;
	std::stack<double> stack_lane_ratings;

	// init lane
	{
		sb::Lane temp_lane;
		temp_lane.parts.push_back( first_lane_part );

		stack_lanes.push( temp_lane );
		stack_lane_ratings.push( 10 ); //** set first rating relative to previous position
	}

	while ( !stack_lanes.empty() ) {

		// pop a bufferred scenario, find and push it new lane part
		sb::Lane lane = stack_lanes.top();
		double lane_rating = stack_lane_ratings.top();

		stack_lanes.pop();
		stack_lane_ratings.pop();

		if ( lane.parts.size() == hops_to_live ) {
			cv::Mat temp_image = image.clone();
			for ( const auto& lane_part: lane.parts ) {
				drawLanePart( lane_part, temp_image, expand_size, cv::Scalar( 255, 255, 255 ), 2 );
			}

			double full_rating = lane_rating / hops_to_live;

			if ( full_rating > 5 ) {
				sb::Road road;
				calculate_full_road( lane, full_rating, full_lines_list, road, temp_image, expand_size );

				lanes_list.push_back( lane );
				lane_ratings_list.push_back( full_rating );
			}

			// debug //
			/*
			cv::Mat temp_image_1 = image.clone();
			for(const auto& lane_part: lane.parts ) {
				drawLanePart( lane_part, temp_image_1, expand_size, cv::Scalar( 255, 255, 255 ), 2 );
			}
			std::stringstream stringBuilder;
			stringBuilder << "Rating: " << lane_rating / hops_to_live;

			cv::putText( temp_image_1, stringBuilder.str(), cv::Point2d( 15, 15 ), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ) );
			cv::imshow( "Analyzer", temp_image_1 );
			cv::waitKey();
			*/
			// debug //
			continue;
		}

		// lastest lane part
		sb::LanePart lastest_lane_part = lane.parts.back();

		// move window to cover center point
		cv::Point2d center_point = (lastest_lane_part.vertices[1] + lastest_lane_part.vertices[2]) * 0.5;

		//** consider use smaller window to find next lane part
		cv::Rect2d window( center_point.x - _windowSize.width / 2,
		                   center_point.y - _windowSize.height / 2, _windowSize.width, _windowSize.height );

		// debug

		cv::Mat temp_image = image.clone();
		// window and old lane parts
		{
			for ( const auto& lane_part : lane.parts ) {
				drawLanePart( lane_part, temp_image, expand_size, cv::Scalar::all( 255 ), 2 );
			}

			cv::rectangle( temp_image,
			               _debugFormatter.convertFromCoord( window.tl() )
			               + cv::Point2d( expand_size.width / 2, expand_size.height ),
			               _debugFormatter.convertFromCoord( window.br() )
			               + cv::Point2d( expand_size.width / 2, expand_size.height ),
			               cv::Scalar( 0, 255, 255 ), 1 );

			//cv::imshow( "Analyzer", temp_image );
			//cv::waitKey( 100 );
		}

		// debug 

		// find lines in window
		std::vector<sb::LineInfo> lines_intersect_window;
		findLinesIntersectRectangle( full_lines_list, window, lines_intersect_window );

		// filter unsuitable lines 
		for ( size_t i = 0; i < lines_intersect_window.size(); ) {
			const auto& line = lines_intersect_window[i];

			double angle = line.getAngle();

			double angle_diff = abs( angle - lastest_lane_part.angle );

			if ( angle_diff > MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_LANE_PARTS ) { //** allow argument to be set
				lines_intersect_window.erase( lines_intersect_window.begin() + i );
				continue;
			}
			i++;
		}

		// debug //
		// lines in window

		{
			cv::Mat temp_image_1 = temp_image.clone();
			for ( const auto& line : lines_intersect_window ) {
				cv::line( temp_image_1,
				          _debugFormatter.convertFromCoord( line.getStartingPoint() )
				          + cv::Point2d( expand_size.width / 2, expand_size.height ),
				          _debugFormatter.convertFromCoord( line.getEndingPoint() )
				          + cv::Point2d( expand_size.width / 2, expand_size.height ), cv::Scalar( 0, 255, 0 ), 1 );
			}
			//cv::imshow( "Analyzer", temp_image_1 );
			//cv::waitKey( 100 );
		}

		// debug //

		// find next lane parts
		std::vector<sb::LanePart> next_lane_parts;
		std::vector<double> next_lane_part_ratings;

		find_next_lane_parts( lines_intersect_window, lastest_lane_part, next_lane_parts, next_lane_part_ratings );

		// none of next part found
		if ( next_lane_parts.empty() ) {
			// copy old lane
			next_lane_parts.push_back( sb::LanePart( lastest_lane_part.vertices[1],
			                                         lastest_lane_part.angle,
			                                         lastest_lane_part.width,
			                                         _windowSize.height ) );
			next_lane_part_ratings.push_back( 0 );
		}

		// push new lanes in to stack
		for ( size_t i = 0; i < next_lane_parts.size(); i++ ) {
			sb::Lane temp_lane;
			temp_lane.parts = std::vector<sb::LanePart>( lane.parts.cbegin(), lane.parts.cend() );
			temp_lane.parts.push_back( next_lane_parts[i] );

			double temp_rating = lane_rating;
			temp_rating += next_lane_part_ratings[i];

			stack_lanes.push( temp_lane );
			stack_lane_ratings.push( temp_rating );
		}
	}

	// chọn ra lane có đánh giá cao nhất
	size_t max_element_index = std::distance( lane_ratings_list.begin(),
	                                          std::max_element( lane_ratings_list.begin(), lane_ratings_list.end() ) );

	//** return -1 nếu đánh giá của các line đều quá thấp (nhỏ hơn thresh)
	if ( lane_ratings_list[max_element_index] < 6 ) return -1;

	output_lane = lanes_list[max_element_index];

	return 0;
}

int sb::Analyzer::calculate_full_road( const sb::Lane& lane,
                                       double lane_rating,
                                       const std::vector<sb::LineInfo>& full_lines_list,
                                       sb::Road& road,
                                       const cv::Mat& image,
                                       const cv::Size& expand_size ) const
{
	// duyệt lane part từ dưới lên
	// tìm làn đối diện bằng cách tìm các đường song song
	// đánh giá cao nếu nó tạo thành làn ở phía đối diện (có 2 đường song song)
	// xác định road width
	// tiếp tục duyệt lên kiểm tra, nhưng sử dụng road width vừa tìm đc

	for ( size_t part_index = 0; part_index < lane.parts.size(); part_index++ ) {
		sb::LanePart part = lane.parts[part_index];

		// find line to consider
		std::vector<sb::LineInfo> lines_to_consider;
		for ( size_t line_index = 0; line_index < full_lines_list.size(); line_index++ ) {
			const sb::LineInfo& line = full_lines_list[line_index];

			double angle = line.getAngle();
			double angle_diff = abs( angle - part.angle );

			if ( angle_diff > MAX_ACCEPTABLE_ANGLE_DIFF_BETWEEN_TWO_LANES ) continue;

			sb::Line horizon( part.vertices[1], part.vertices[2] );

			cv::Point2d knot;
			sb::Line::findIntersection( horizon, line.getLine(), knot );

			cv::Point2d pos_diff = part.vertices[1] - knot;
			double pos_distance = std::sqrt( pos_diff.x * pos_diff.x + pos_diff.y * pos_diff.y );

			if ( pos_distance < _minRoadWidth || pos_distance > _maxRoadWidth + part.width * 2 ) continue;

			lines_to_consider.push_back( line );
		}

		// debug //
		{
			cv::Mat temp_image = image.clone();
			drawLanePart( part, temp_image, expand_size, cv::Scalar( 0, 255, 255 ), 2 );
			for ( const auto& line: lines_to_consider ) {
				cv::line( temp_image,
				          _debugFormatter.convertFromCoord( line.getStartingPoint() )
				          + cv::Point2d( expand_size.width / 2, expand_size.height ),
				          _debugFormatter.convertFromCoord( line.getEndingPoint() )
				          + cv::Point2d( expand_size.width / 2, expand_size.height ), cv::Scalar( 0, 255, 0 ), 1 );
			}
			cv::imshow( "Analyzer", temp_image );
			cv::waitKey();
		}
		// debug //
	}

	return 0;
}

int sb::Analyzer::analyze3( const sb::FrameInfo& frameInfo, sb::RoadInfo& roadInfo ) const
{
	const size_t num_lines = frameInfo.getRealLineInfos().size();

	///// debug //////
	sb::Timer timer;
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

		cv::imshow( "Analyzer", real_image );
		cv::waitKey();
	}
	///// debug //////

	// window //** consider use bigger window to find first lane part
	cv::Rect2d window( -_topRightCorner.x, 5, _windowSize.width, _windowSize.height );

	do {

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

		std::vector<sb::LanePart> first_lane_parts;
		findFirstLaneParts( frameInfo.getRealLineInfos(), window, first_lane_parts, temp_image, expand_size );

		// debug //
		{
			for ( const auto& lane_part: first_lane_parts ) {
				cv::Mat img = temp_image.clone();
				drawLanePart( lane_part, img, expand_size, cv::Scalar( 255, 255, 255 ), 2 );
				// cv::imshow( "Analyzer", img );
				// cv::waitKey( 200 );
			}
		}

		for ( const auto& first_lane_part : first_lane_parts ) {
			std::vector<sb::Lane> lanes;
			std::vector<double> lane_ratings;

			timer.reset( "findLanes" );
			findLanes( frameInfo.getRealLineInfos(), first_lane_part, lanes, lane_ratings, temp_image, expand_size );
			std::cout << timer.milliseconds( "findLanes" ) << std::endl;

			for(size_t lane_index = 0; lane_index < lanes.size(); lane_index++ ) {
				const sb::Lane& lane = lanes[lane_index];
				const double lane_rating = lane_ratings[lane_index];

				// debug //
				{
					cv::Mat img = temp_image.clone();
					for( const auto& lane_part : lane.parts ) {
						drawLanePart( lane_part, img, expand_size, cv::Scalar( 0, 255, 255 ), 2 );
					}
					cv::imshow( "Analyzer", img );
					cv::waitKey();
				}

				std::vector<sb::Road> roads;
				std::vector<double> road_ratings;

				findRoads( frameInfo.getRealLineInfos(), lane, lane_rating, roads, road_ratings, temp_image, expand_size );
			}
		}

		// move window around frame to find lane candidate until it meets some requirements
		if ( moveMainWindow( window ) < 0 ) break;

	} while ( true );

	return 0;
}

void sb::Analyzer::findFirstLaneParts( const std::vector<sb::LineInfo>& line_lists,
                                       const cv::Rect2d& window,
                                       std::vector<sb::LanePart>& first_lane_parts,
                                       const cv::Mat& image,
                                       const cv::Size& expand_size ) const
{
	first_lane_parts.clear();

	std::vector<sb::LineInfo> lines_intersect_window;
	findLinesIntersectRectangle( line_lists, window, lines_intersect_window );

	const size_t n_lines = lines_intersect_window.size();

	for ( size_t first_index = 0; first_index < n_lines; first_index++ ) {
		const auto& first_line = lines_intersect_window[first_index];

		for ( size_t second_index = first_index + 1; second_index < n_lines; second_index++ ) {
			const auto& second_line = lines_intersect_window[second_index];

			/// 1) check and calculate lane info

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

			/// 2) add to good couples
			sb::LanePart lane_part( pos, angle, width, _lanePartLength );

			if ( std::find( first_lane_parts.cbegin(), first_lane_parts.cend(), lane_part )
				!= first_lane_parts.cend() )
				continue;

			first_lane_parts.push_back( lane_part );
		}
	}
}

void sb::Analyzer::findLanes( const std::vector<sb::LineInfo>& lines_list,
                              const sb::LanePart& first_lane_part,
                              std::vector<sb::Lane>& lanes,
                              std::vector<double>& lane_ratings,
                              const cv::Mat& image,
                              const cv::Size& expand_size ) const
{
	int hops_to_live = 7; //** need to be set

	std::stack<sb::Lane> stack_lanes;
	std::stack<double> stack_lane_ratings;

	// init lane
	{
		sb::Lane temp_lane;
		temp_lane.parts.push_back( first_lane_part );

		stack_lanes.push( temp_lane );
		stack_lane_ratings.push( 10 ); //** set first rating relative to previous position
	}

	while ( !stack_lanes.empty() ) {

		// pop a bufferred scenario, find and push it new lane part
		sb::Lane lane = stack_lanes.top();
		double lane_rating = stack_lane_ratings.top();

		stack_lanes.pop();
		stack_lane_ratings.pop();

		// lastest lane part
		sb::LanePart lastest_lane_part = lane.parts.back();

		// move window to cover center point
		cv::Point2d window_origin = (lastest_lane_part.vertices[1] + lastest_lane_part.vertices[2]) * 0.5;

		//** consider use smaller window to find next lane part
		cv::Rect2d window( window_origin.x - _windowSize.width / 2,
		                   window_origin.y - _windowSize.height / 2, _windowSize.width, _windowSize.height );

		// debug
		{
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

			// cv::imshow( "Analyzer", temp_image );
			// cv::waitKey( 200 );
		}

		// finish a lane
		if ( lane.parts.size() == hops_to_live ) {
			double full_rating = lane_rating / hops_to_live;

			lanes.push_back( lane );
			lane_ratings.push_back( full_rating );
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

		findNextLaneParts( lines_intersect_window, lastest_lane_part, next_lane_parts, next_lane_part_ratings );

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

			double temp_rating = lane_rating;
			temp_rating += next_lane_part_ratings[i];

			stack_lanes.push( temp_lane );
			stack_lane_ratings.push( temp_rating );
		}
	}
}

void sb::Analyzer::findNextLaneParts( const std::vector<sb::LineInfo>& lines_list,
                                      const sb::LanePart& lastest_lane_part,
                                      std::vector<sb::LanePart>& next_lane_parts,
                                      std::vector<double>& next_lane_part_ratings ) const
{
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

			next_lane_parts.push_back( lane_part );
			next_lane_part_ratings.push_back( rating );
		}
	}
}

void sb::Analyzer::findRoads( const std::vector<sb::LineInfo>& lines_list,
															const sb::Lane& lane,
															const double& lane_rating,
															std::vector<sb::Road>& roads,
                              std::vector<double>& road_ratings,
                              const cv::Mat& image,
                              const cv::Size& expand_size ) const
{
	// find list of possible side and road width

}

int sb::Analyzer::moveMainWindow( cv::Rect2d& window ) const
{
	window.x += _windowMove.x;
	if( window.x + window.width > _topRightCorner.x ) {
		window.x = -_topRightCorner.x;
		window.y += _windowMove.y;
		if( window.y > _topRightCorner.y ) return -1;
	}

	return 0;
}

int sb::Analyzer::drawLanePart( const sb::LanePart& lane_part, cv::Mat& image,
																const cv::Size& expand_size,
																const cv::Scalar& color,
																int line_width ) const
{
	for( int v = 0; v < 4; v++ ) {
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
	for( size_t line_index = 0; line_index < inputLines.size(); line_index++ ) {
		const sb::LineInfo& line = inputLines[line_index];
		if( !segmentIntersectRectangle( line.getStartingPoint(), line.getEndingPoint(), window ) ) continue;

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

	if( p1.x > p2.x ) {
		minX = p2.x;
		maxX = p1.x;
	}

	// Find the intersection of the segment's and rectangle's x-projections

	if( maxX > rect.x + rect.width ) {
		maxX = rect.x + rect.width;
	}

	if( minX < rect.x ) {
		minX = rect.x;
	}

	if( minX > maxX ) // If their projections do not intersect return false
	{
		return false;
	}

	// Find corresponding min and max Y for min and max X we found before

	double minY = p1.y;
	double maxY = p2.y;

	double dx = p2.x - p1.x;

	if( abs( dx ) > 0.0000001 ) {
		double a = (p2.y - p1.y) / dx;
		double b = p1.y - a * p1.x;
		minY = a * minX + b;
		maxY = a * maxX + b;
	}

	if( minY > maxY ) {
		double tmp = maxY;
		maxY = minY;
		minY = tmp;
	}

	// Find the intersection of the segment's and rectangle's y-projections

	if( maxY > rect.y + rect.height ) {
		maxY = rect.y + rect.height;
	}

	if( minY < rect.y ) {
		minY = rect.y;
	}

	if( minY > maxY ) // If Y-projections do not intersect return false
	{
		return false;
	}

	return true;
}