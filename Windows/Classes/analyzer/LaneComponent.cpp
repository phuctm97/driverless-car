#include "LaneComponent.h"

#ifdef SB_DEBUG
void sb::LaneComponent::init( int side, double minLaneWidth, double maxLaneWidth, const sb::Params& params )
#else
void sb::LaneComponent::init( int side, double minLaneWidth, double maxLaneWidth )
#endif // init()
{
	_initSucceeded = false;

	_side = side;

	_minLandWidth = minLaneWidth;
	_maxLandWidth = maxLaneWidth;

	_windowSize = cv::Size( static_cast<int>(_maxLandWidth) * 2, static_cast<int>(_maxLandWidth) );
	_windowMove = cv::Point2d( _maxLandWidth, _maxLandWidth );
	_windowMaxXY = cv::Point2d( 100, 50 );
	_lanePartLength = 12;

#ifdef SB_DEBUG
	{
		_debugFormatter = sb::Formatter( params.CROP_BOX,
		                                 params.WARP_SRC_QUAD,
		                                 params.WARP_DST_QUAD,
		                                 params.CONVERT_COORD_COEF );
	}
#endif //SB_DEBUG
}

int sb::LaneComponent::findItself( const sb::FrameInfo& frameInfo )
{
#ifdef SB_DEBUG
	const cv::Size expand_size( 900, 700 );
	cv::Mat debug_image;
	// create real debug_image
	{
		debug_image = cv::Mat( frameInfo.getColorImage().rows + expand_size.height,
		                       frameInfo.getColorImage().cols + expand_size.width, CV_8UC3, cv::Scalar( 0, 0, 0 ) );

		cv::line( debug_image,
		          _debugFormatter.convertFromCoord( cv::Point2d( 0, 0 ) )
		          + cv::Point2d( expand_size.width / 2, expand_size.height ),
		          _debugFormatter.convertFromCoord( cv::Point2d( 0, 500 ) )
		          + cv::Point2d( expand_size.width / 2, expand_size.height ),
		          cv::Scalar( 255, 255, 255 ), 1 );

		for ( size_t i = 0; i < frameInfo.getRealLineInfos().size(); i++ ) {
			const auto& line = frameInfo.getRealLineInfos()[i];

			cv::line( debug_image,
			          _debugFormatter.convertFromCoord( line.getStartingPoint() )
			          + cv::Point2d( expand_size.width / 2, expand_size.height ),
			          _debugFormatter.convertFromCoord( line.getEndingPoint() )
			          + cv::Point2d( expand_size.width / 2, expand_size.height ), cv::Scalar( 0, 0, 255 ), 1 );
		}
	}
#endif // draw lines

	cv::Rect2d window;
	if ( _side < 0 )
		window = cv::Rect2d( -_windowSize.width, 0, _windowSize.width, _windowSize.height );
	else if ( _side > 0 )
		window = cv::Rect2d( 0, 0, _windowSize.width, _windowSize.height );

	sb::Lane final_lane;
	final_lane.rating = 0;

	do {

#ifdef SB_DEBUG
		cv::Mat temp_image; {
			temp_image = debug_image.clone();
			cv::rectangle( temp_image,
			               _debugFormatter.convertFromCoord( window.tl() )
			               + cv::Point2d( expand_size.width / 2, expand_size.height ),
			               _debugFormatter.convertFromCoord( window.br() )
			               + cv::Point2d( expand_size.width / 2, expand_size.height ), cv::Scalar::all( 255 ) );
			cv::imshow( "Analyzer", temp_image );
			cv::waitKey();
		}
#endif //draw window

		std::vector<sb::LanePart> first_lane_parts;

#ifdef SB_DEBUG
		findFirstLaneParts( frameInfo.getRealLineInfos(), window, first_lane_parts, temp_image, expand_size );
#else
		findFirstLaneParts( frameInfo.getRealLineInfos(), window, first_lane_parts );
#endif // findFirstLaneParts()

		/*#ifdef SB_DEBUG
				{
					for ( const auto& lane_part: first_lane_parts ) {
						cv::Mat img = temp_image.clone();
						drawLanePart( lane_part, img, expand_size, cv::Scalar( 255, 255, 255 ), 2 );
						cv::imshow( "Analyzer", img );
						cv::waitKey( 100 );
					}
				}
		#endif *///show first lane parts

		for ( const auto& first_lane_part : first_lane_parts ) {
			std::vector<sb::Lane> lanes;

#ifdef SB_DEBUG
			findLanes( frameInfo.getRealLineInfos(), first_lane_part, lanes, temp_image, expand_size );
#else
			findLanes( frameInfo.getRealLineInfos(), first_lane_part, lanes );
#endif // findLanes()

			if ( lanes.empty() )continue;

			if ( lanes.front().rating > final_lane.rating )
				final_lane = lanes.front();
		}

		if ( moveFirstWindow( window ) < 0 ) break;

	} while ( true );

	if ( final_lane.rating <= BAD_LINE_RATING ) return -1;

	_position = final_lane.parts[0].position;

	_width = final_lane.width;

	_parts = final_lane.parts;

	_angle = 0;
	for ( const auto& part : final_lane.parts ) _angle += part.angle;
	_angle /= final_lane.parts.size();

	_initSucceeded = true;

	return 0;
}

int sb::LaneComponent::track( const sb::FrameInfo& frameInfo )
{
	if ( !_initSucceeded ) return -1;

	return 0;
}

#ifdef SB_DEBUG
void sb::LaneComponent::findFirstLaneParts( const std::vector<sb::LineInfo>& line_lists,
                                            const cv::Rect2d& window,
                                            std::vector<sb::LanePart>& first_lane_parts,
                                            const cv::Mat& image,
                                            const cv::Size& expand_size ) const
#else
void sb::LaneComponent::findFirstLaneParts( const std::vector<sb::LineInfo>& line_lists,
												 const cv::Rect2d& window,
												 std::vector<sb::LanePart>& first_lane_parts ) const
#endif // findFirstLaneParts()
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
			{
				bool existedSimilarPart = false;
				for ( const auto& other : first_lane_parts ) {
					double angle_diff_with_other = abs( angle - other.angle );
					double width_diff_with_other = abs( width - other.width );

					cv::Point2d pos_diff_with_other = pos - other.position;
					double pos_distance_with_other = std::sqrt( pos_diff_with_other.x * pos_diff_with_other.x +
					                                           pos_diff_with_other.y * pos_diff_with_other.y );

					existedSimilarPart = (angle_diff_with_other <= 1 &&
						width_diff_with_other <= 1 &&
						pos_distance_with_other <= 1);

					if ( existedSimilarPart ) break;
				}
				if ( existedSimilarPart ) continue;
			}

			first_lane_parts.push_back( lane_part );
		}
	}
}

#ifdef SB_DEBUG
void sb::LaneComponent::findNextLaneParts( const std::vector<sb::LineInfo>& lines_list,
                                           const sb::LanePart& lastest_lane_part,
                                           std::vector<sb::LanePart>& next_lane_parts,
                                           std::vector<double>& next_lane_part_ratings,
                                           const cv::Mat& image,
                                           const cv::Size& expand_size ) const
#else
void sb::LaneComponent::findNextLaneParts( const std::vector<sb::LineInfo>& lines_list,
												const sb::LanePart& lastest_lane_part,
												std::vector<sb::LanePart>& next_lane_parts,
												std::vector<double>& next_lane_part_ratings ) const
#endif // findNextLaneParts()
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

			// check whether there has been a lane part similar to this
			{
				bool existedSimilarPart = false;
				for ( const auto& other : next_lane_parts ) {
					double angle_diff_with_other = abs( angle - other.angle );
					double width_diff_with_other = abs( width - other.width );

					cv::Point2d pos_diff_with_other = pos - other.position;
					double pos_distance_with_other = std::sqrt( pos_diff_with_other.x * pos_diff_with_other.x +
					                                           pos_diff_with_other.y * pos_diff_with_other.y );

					existedSimilarPart = (angle_diff_with_other <= 1 &&
						width_diff_with_other <= 1 &&
						pos_distance_with_other <= 1);

					if ( existedSimilarPart ) break;
				}
				if ( existedSimilarPart ) continue;
			}

			//** consider new function
			double width_rating = 10 - (10.0 / MAX_ACCEPTABLE_WIDTH_DIFF_BETWEEN_LANE_PARTS) * width_diff_with_lastest;
			double pos_rating = 10 - (10.0 / MAX_ACCEPTABLE_DISTANCE_BETWEEN_LANE_PARTS) * pos_distance_with_lastest;

			double rating = 0.65 * width_rating + 0.35 * pos_rating;

			next_lane_parts.push_back( lane_part );
			next_lane_part_ratings.push_back( rating );
		}
	}
}

#ifdef SB_DEBUG
void sb::LaneComponent::findLanes( const std::vector<sb::LineInfo>& lines_list,
                                   const sb::LanePart& first_lane_part,
                                   std::vector<sb::Lane>& lanes,
                                   const cv::Mat& debug_image,
                                   const cv::Size& expand_size ) const
#else
void sb::LaneComponent::findLanes( const std::vector<sb::LineInfo>& lines_list,
																	 const sb::LanePart& first_lane_part,
																	 std::vector<sb::Lane>& lanes ) const
#endif // findLanes()
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
		{
			cv::Mat img = debug_image.clone();

			cv::rectangle( img,
			               _debugFormatter.convertFromCoord( window.tl() )
			               + cv::Point2d( expand_size.width / 2, expand_size.height ),
			               _debugFormatter.convertFromCoord( window.br() )
			               + cv::Point2d( expand_size.width / 2, expand_size.height ),
			               cv::Scalar( 0, 255, 255 ), 1 );

			for ( const auto& lane_part : lane.parts ) {
				drawLanePart( lane_part, img, expand_size, cv::Scalar::all( 255 ), 2 );
			}

			cv::imshow( "Analyzer", img );
			cv::waitKey( 100 );
		}
#endif //show window and sequence of lane parts

		// finish a lane
		if ( lane.parts.size() == hops_to_live ) {
			lane.rating /= hops_to_live;

			if ( lane.rating > BAD_LINE_RATING ) {

#ifdef SB_DEBUG
				{
					cv::Mat img = debug_image.clone();
					for ( const auto& lane_part : lane.parts ) {
						drawLanePart( lane_part, img, expand_size, cv::Scalar( 0, 255, 255 ), 2 );
					}
					std::stringstream stringBuilder;
					stringBuilder << "Rating: " << lane.rating;
					cv::putText( img, stringBuilder.str(), cv::Point2d( 30, 30 ), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ) );
					cv::imshow( "Analyzer", img );
					cv::waitKey( 200 );
				}
#endif //show completed lane

				lanes.push_back( lane );
				// if ( lane.rating >= GOOD_LANE_RATING ) break;
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
		findNextLaneParts( lines_intersect_window, lastest_lane_part, next_lane_parts, next_lane_part_ratings, debug_image, expand_size );
#else
		findNextLaneParts( lines_intersect_window, lastest_lane_part, next_lane_parts, next_lane_part_ratings );
#endif // findNextLaneParts()

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

int sb::LaneComponent::findLinesIntersectRectangle( const std::vector<sb::LineInfo>& inputLines,
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

bool sb::LaneComponent::segmentIntersectRectangle( const cv::Point2d& p1,
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

int sb::LaneComponent::moveFirstWindow( cv::Rect2d& window ) const
{
	if ( _side < 0 ) {
		window.x -= _windowMove.x;
		if ( window.x < -_windowMaxXY.x ) {
			return -1;

			/*window.x = -_windowSize.width;
			window.y += _windowMove.y;
			if( window.y > _windowMaxXY.y ) return -1;*/
		}
	}
	else if ( _side > 0 ) {
		window.x += _windowMove.x;
		if ( window.x + window.width > _windowMaxXY.x ) {
			return -1;
			/*window.x = 0;
			window.y += _windowMove.y;
			if( window.y > _windowMaxXY.y ) return -1;*/
		}
	}
	else return -1;

	return 0;
}

#ifdef SB_DEBUG
int sb::LaneComponent::drawLanePart( const sb::LanePart& lane_part,
                                     cv::Mat& image, const cv::Size& expand_size,
                                     const cv::Scalar& color, int line_width ) const
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
#endif //drawLanePart()
