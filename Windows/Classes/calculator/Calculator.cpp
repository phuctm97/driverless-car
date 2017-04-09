#include "Calculator.h"
#include "../Timer.h"

int sb::Calculator::init( const sb::Params& params )
{
	_edgeDetector = sb::EdgeDetector( params.EDGE_DETECTOR_KERNEL_SIZE,
	                                  params.EDGE_DETECTOR_LOW_THRESH,
	                                  params.EDGE_DETECTOR_HIGH_THRESH,
	                                  params.BINARIZE_THRESH,
	                                  params.BINARIZE_MAX_VALUE );

	_lineDetector = sb::LineDetector( params.HOUGH_LINES_P_RHO,
	                                  params.HOUGH_LINES_P_THETA,
	                                  params.HOUGH_LINES_P_THRESHOLD,
	                                  params.HOUGH_LINES_P_MIN_LINE_LENGTH,
	                                  params.HOUGH_LINES_P_MAX_LINE_GAP );

	_cropBox = params.CROP_BOX;

	std::vector<double> _splitRatio = { 0.2, 0.25, 0.25, 0.3 };

	_splitBoxes.clear(); {
		int y = 0;
		for ( double ratio : _splitRatio ) {
			int h = static_cast<int>(round( 1.0 * params.CROP_BOX.height * ratio ));

			if ( y + h > params.CROP_BOX.height ) h = params.CROP_BOX.height - y;

			cv::Rect box( 0, y, params.CROP_BOX.width, h );
			_splitBoxes.push_back( box );
			y += h;
		}
	}

	return 0;
}

int sb::Calculator::calculate( const sb::RawContent& rawContent,
                               sb::FrameInfo& frameInfo ) const
{
	// 1) color image
	cv::Mat colorImage;

	// crop to correct format
	if ( _cropBox.x < 0 || _cropBox.y < 0 ||
		_cropBox.x + _cropBox.width > rawContent.getColorImage().cols ||
		_cropBox.y + _cropBox.height > rawContent.getColorImage().rows ) {
		std::cerr << "Input image and crop box aren't suitable." << std::endl;
		return -1;
	}
	colorImage = rawContent.getColorImage()( _cropBox );

	// flip to natural direction ( input image from camera was flipped )
	cv::flip( colorImage, colorImage, 1 );

	frameInfo.setColorImage( colorImage );

	// 2) generate edges-frame
	cv::Mat edgesFrame;
	cv::cvtColor( colorImage, edgesFrame, cv::COLOR_BGR2GRAY );
	_edgeDetector.apply( edgesFrame );

	frameInfo.setEdgesImage( edgesFrame );

	// 3) generate sections
	std::vector<sb::Section> sections;
	sections.reserve( 5 );
	for ( auto it = _splitBoxes.crbegin(); it != _splitBoxes.crend(); ++it ) {
		sections.push_back( sb::Section( edgesFrame, *it ) );
	}

	// 4) generate lines for each section

	for ( auto it_section = sections.begin(); it_section != sections.end(); ++it_section ) {
		std::vector<sb::Line> lines;
		_lineDetector.apply( it_section->getBinaryImage(), lines );

		std::vector<sb::LineInfo> lineInfos;
		lineInfos.reserve( lines.size() );

		for ( auto it_line = lines.cbegin(); it_line != lines.cend(); ++it_line ) {
			sb::Line translatedLine( it_section->convertToContainerSpace( it_line->getStartingPoint() ),
			                         it_section->convertToContainerSpace( it_line->getEndingPoint() ) );

			sb::LineInfo lineInfo( translatedLine );

			cv::Point2d topPoint, bottomPoint;
			if ( !sb::Line::findIntersection( it_section->getTopLine(), translatedLine, topPoint ) ) continue;
			if ( !sb::Line::findIntersection( it_section->getBottomLine(), translatedLine, bottomPoint ) ) continue;

			lineInfo.setTopPoint( topPoint );
			lineInfo.setBottomPoint( bottomPoint );
			lineInfo.setCenterPoint( (topPoint + bottomPoint) * 0.5 );

			lineInfos.push_back( lineInfo );
		}

		std::sort( lineInfos.begin(), lineInfos.end(),
		           [](const sb::LineInfo& l1, const sb::LineInfo& l2)-> bool {
			           return l1.getBottomPoint().x < l2.getBottomPoint().x;
		           } );

		it_section->setImageLines( lineInfos );
	}
	frameInfo.setImageSections( sections );

	return 0;
}

void sb::Calculator::release() {}
