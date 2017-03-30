#include "Analyzer.h"
#include "../collector/RawContent.h"

int sb::Analyzer::init( const sb::Params& params )
{
	return 0;
}

int sb::Analyzer::analyze( const sb::FrameInfo& frameInfo, sb::RoadInfo& roadInfo ) const
{
	const int N_LINES = static_cast<int>(frameInfo.getWarpedLines().size());
	const int N_SECTIONS = static_cast<int>(roadInfo.getRotationOfLanes().size());

	///// Old values /////
	std::vector<double> OLD_ROTATIONS = roadInfo.getRotationOfLanes();
	std::vector<double> OLD_LEFT_POSITIONS( N_SECTIONS + 1, 0 );
	std::vector<double> OLD_RIGHT_POSITIONS( N_SECTIONS + 1, 0 );

	OLD_LEFT_POSITIONS[0] = roadInfo.getPositionOfLeftLane();
	OLD_RIGHT_POSITIONS[0] = roadInfo.getPositionOfRightLane();

	for ( int i = 0; i < N_SECTIONS; i++ ) {
		const int upperRow = frameInfo.getSections()[i].upperRow;
		const int lowerRow = frameInfo.getSections()[i].lowerRow;

		const sb::Line upperLine( cv::Point2d( 0, upperRow ), cv::Point2d( 1, upperRow ) );

		sb::Line line;
		cv::Point2d p;

		line = sb::Line( frameInfo.convertFromRotation( OLD_ROTATIONS[i] ),
		                 cv::Point2d( frameInfo.convertXFromCoord( OLD_LEFT_POSITIONS[i] ), lowerRow ) );

		sb::Line::findIntersection( line, upperLine, p );
		OLD_LEFT_POSITIONS[i + 1] = frameInfo.convertXToCoord( p.x );

		line = sb::Line( frameInfo.convertFromRotation( OLD_ROTATIONS[i] ),
		                 cv::Point2d( frameInfo.convertXFromCoord( OLD_RIGHT_POSITIONS[i] ), lowerRow ) );

		sb::Line::findIntersection( line, upperLine, p );
		OLD_RIGHT_POSITIONS[i + 1] = frameInfo.convertXToCoord( p.x );
	}

	///// Ratings /////
	std::vector<double> _lineRatings( N_LINES, 0 );
	std::vector<double> _sideRatings( N_LINES, 0 );

	// independent ratings in whole frame
	for ( int i = 0; i < N_LINES; i++ ) {
		const sb::LineInfo& line = frameInfo.getLines()[i];
		const sb::LineInfo& warpedLine = frameInfo.getWarpedLines()[i];

		// length
		_lineRatings[i] += warpedLine.getLength();

		//** color
		_lineRatings[i] += 5.0 * (line.getAverageColor()[0] + line.getAverageColor()[1] + line.getAverageColor()[2]) / 3;
	}

	// independent ratings in section
	for ( int i = 0; i < N_SECTIONS; i++ ) {
		const sb::SectionInfo& sectionInfo = frameInfo.getSections()[i];
		const int n_lines = static_cast<int>(sectionInfo.lines.size());

		for ( int j = 0; j < n_lines; j++ ) {
			const std::pair<int, cv::Vec2d>& sectionLineInfo = sectionInfo.lines[j];
			const sb::LineInfo warpedLine = frameInfo.getWarpedLines()[sectionLineInfo.first];

			const int lineIndex = sectionLineInfo.first;
			const double upperX = frameInfo.convertXToCoord( sectionLineInfo.second[0] );
			const double lowerX = frameInfo.convertXToCoord( sectionLineInfo.second[1] );
			const double rotation = frameInfo.convertToRotation( warpedLine.getAngle() );

			//** position and rotation
			if ( abs( lowerX - OLD_LEFT_POSITIONS[i] ) < 0.25 ) {
				_sideRatings[lineIndex] -= 1000;
				_lineRatings[lineIndex] += 100;
				if ( abs( rotation - OLD_ROTATIONS[i] ) < 15 ) {
					_lineRatings[lineIndex] += 1000;
				}
			}
			else if ( abs( lowerX - OLD_RIGHT_POSITIONS[i] ) < 0.25 ) {
				_sideRatings[lineIndex] += 1000;
				_lineRatings[lineIndex] += 100;
				if ( abs( rotation - OLD_ROTATIONS[i] ) < 15 ) {
					_lineRatings[lineIndex] += 1000;
				}
			}
		}

	}

	// dependent ratings
	for ( int i = 0; i < N_SECTIONS; i++ ) {
		const sb::SectionInfo& sectionInfo = frameInfo.getSections()[i];
		const int n_lines = static_cast<int>(sectionInfo.lines.size());

		for ( int j = 0; j < n_lines; j++ ) {
			const std::pair<int, cv::Vec2d>& sectionLineInfo1 = sectionInfo.lines[j];
			const sb::LineInfo warpedLine1 = frameInfo.getWarpedLines()[sectionLineInfo1.first];

			double upperX1 = frameInfo.convertXToCoord( sectionLineInfo1.second[0] );
			double lowerX1 = frameInfo.convertXToCoord( sectionLineInfo1.second[1] );
			double rotation1 = frameInfo.convertToRotation( warpedLine1.getAngle() );

			for ( int k = 0; k < n_lines; k++ ) {
				if ( j == k ) continue;

				const std::pair<int, cv::Vec2d>& sectionLineInfo2 = sectionInfo.lines[k];
				const sb::LineInfo warpedLine2 = frameInfo.getWarpedLines()[sectionLineInfo2.first];

				double upperX2 = frameInfo.convertXToCoord( sectionLineInfo2.second[0] );
				double lowerX2 = frameInfo.convertXToCoord( sectionLineInfo2.second[1] );
				double rotation2 = frameInfo.convertToRotation( warpedLine2.getAngle() );

			}
		}
	}

	///// Debug /////


	
	
	/*// create warped image
	const int H = static_cast<int>(abs( frameInfo.getTopLeftPoint().y ));
	const int W = static_cast<int>(abs( frameInfo.getTopLeftPoint().x )) * 2;
	cv::Mat warpedImage( frameInfo.getColorImage().rows + H,
	                     frameInfo.getColorImage().cols + W, CV_8UC3, cv::Scalar( 0, 0, 0 ) );

	for ( const auto& line : frameInfo.getWarpedLines() ) {
		cv::line( warpedImage,
		          line.getStartingPoint() + cv::Point2d( W / 2, H ),
		          line.getEndingPoint() + cv::Point2d( W / 2, H ),
		          cv::Scalar( 0, 0, 255 ), 1 );
	}
	for ( int i = 0; i < N_SECTIONS + 1; i++ ) {
		cv::circle( warpedImage,
		            cv::Point2d( frameInfo.convertXFromCoord( OLD_LEFT_POSITIONS[i] ),
		                         i < N_SECTIONS ? frameInfo.getSections()[i].lowerRow : frameInfo.getSections()[i - 1].upperRow ) + cv::Point2d( W / 2, H ),
		            3, cv::Scalar( 255, 255, 255 ), -1 );
		cv::circle( warpedImage,
		            cv::Point2d( frameInfo.convertXFromCoord( OLD_RIGHT_POSITIONS[i] ),
		                         i < N_SECTIONS ? frameInfo.getSections()[i].lowerRow : frameInfo.getSections()[i - 1].upperRow ) + cv::Point2d( W / 2, H ),
		            3, cv::Scalar( 255, 255, 255 ), -1 );
	}

	// debug each lines
	for ( int i = 0; i < N_LINES; i++ ) {
		const sb::LineInfo& line = frameInfo.getLines()[i];
		const sb::LineInfo& warpedLine = frameInfo.getWarpedLines()[i];

		cv::Mat tempImage = warpedImage.clone();
		cv::line( tempImage,
		          warpedLine.getStartingPoint() + cv::Point2d( W / 2, H ),
		          warpedLine.getEndingPoint() + cv::Point2d( W / 2, H ),
		          cv::Scalar( 0, 255, 0 ), 2 );

		std::stringstream stringBuilder;

		stringBuilder << "Line Rating: " << _lineRatings[i];
		cv::putText( tempImage,
		             stringBuilder.str(),
		             cv::Point( 20, 15 ),
		             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

		stringBuilder.str( "" );

		stringBuilder << "Side Rating: " << _sideRatings[i];
		cv::putText( tempImage,
		             stringBuilder.str(),
		             cv::Point( 20, 35 ),
		             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

		cv::imshow( "Test2", tempImage );
		cv::waitKey();
	}*/

	///// Update /////

	//** situation for no lines
	
	// position of lanes
	{
		double sumPositionXOfLeftLane = 0;
		double sumPositionXOfRightLane = 0;
		int n1 = 0;
		int n2 = 0;
		for( int i = 0; i < frameInfo.getSections().front().lines.size(); i++ ) {
			int lineIndex = frameInfo.getSections().front().lines[i].first;

			if( _lineRatings[lineIndex] < 1500 ) continue;

			if( _sideRatings[lineIndex] == 0 ) continue;

			if( _sideRatings[lineIndex] < 0 ) {
				sumPositionXOfLeftLane += frameInfo.convertXToCoord( frameInfo.getSections().front().lines[i].second[1] );
				n1++;
			}
			else {
				sumPositionXOfRightLane += frameInfo.convertXToCoord( frameInfo.getSections().front().lines[i].second[1] );
				n2++;
			}
		}
		roadInfo.setPositionOfLeftLane( sumPositionXOfLeftLane / n1 );
		roadInfo.setPositionOfRightLane( sumPositionXOfRightLane / n2 );
	}

	// rotation of lanes
	{
		std::vector<double> rotationOfLanes( N_SECTIONS, 0 );
		for( int i = 0; i < N_SECTIONS; i++ ) {
			const sb::SectionInfo& sectionInfo = frameInfo.getSections()[i];

			double sumRotation = 0;
			int n = 0;

			for( int j = 0; j < sectionInfo.lines.size(); j++ ) {
				int lineIndex = sectionInfo.lines[j].first;

				if( _lineRatings[lineIndex] < 1500 ) continue;

				sumRotation += frameInfo.convertToRotation( frameInfo.getWarpedLines()[lineIndex].getAngle() );
				n++;
			}

			rotationOfLanes[i] = sumRotation / n;
		}
		roadInfo.setRotationOfLanes( rotationOfLanes );
	}

	return 0;
}

void sb::Analyzer::release() { }

int sb::Analyzer::analyze1( const sb::FrameInfo& frameInfo, sb::RoadInfo& roadInfo ) const
{
	const double FRAME_HALF_WIDTH = frameInfo.getColorImage().cols / 2;

	const double OLD_ROTATION = roadInfo.getRotationOfLanes()[0];
	const double OLD_POSITION_X_OF_LEFT_LANE = roadInfo.getPositionOfLeftLane();
	const double OLD_POSITION_X_OF_RIGHT_LANE = roadInfo.getPositionOfRightLane();

	double sumRotation = 0;
	double sumPositionXOfLeftLane = 0;
	double sumPositionXOfRightLane = 0;

	int n1 = 0;
	int n2 = 0;
	int n3 = 0;

	for ( int i = 0; i < static_cast<int>(frameInfo.getWarpedLines().size()); i++ ) {

		const sb::LineInfo& line = frameInfo.getWarpedLines()[i];

		double rotation = 90 - line.getAngle();
		double positionX = line.getEndingPoint().x / FRAME_HALF_WIDTH - 1;

		if ( abs( rotation - OLD_ROTATION ) > 20 ) continue;

		if ( positionX < 0 ) {
			if ( abs( positionX - OLD_POSITION_X_OF_LEFT_LANE ) > 0.2 ) continue;;

			sumRotation += rotation;
			n1++;

			sumPositionXOfLeftLane += positionX;
			n2++;
		}
		else {
			if ( abs( positionX - OLD_POSITION_X_OF_RIGHT_LANE ) > 0.2 )continue;

			sumRotation += rotation;
			n1++;

			sumPositionXOfRightLane += positionX;
			n3++;
		}
	}

	double rotation = sumRotation / n1;
	double positionXOfLeftLane = sumPositionXOfLeftLane / n2;
	double positionXOfRightLane = sumPositionXOfRightLane / n3;

	roadInfo.setRotationOfLanes( std::vector<double>( 1, rotation ) );
	roadInfo.setPositionOfLeftLane( positionXOfLeftLane );
	roadInfo.setPositionOfRightLane( positionXOfRightLane );

	return 0;
}
