#include "Calculator.h"

void sb::construct( sb::Calculator*& calculator )
{
	calculator = new Calculator;
}

void sb::destruct( sb::Calculator*& calculator )
{
	delete calculator;
	calculator = nullptr;
}

int sb::init( sb::Calculator* calculator, sb::Params* params )
{
	calculator->edgeDetector = sb::EdgeDetector( params->EDGE_DETECTOR_KERNEL_SIZE,
	                                             params->EDGE_DETECTOR_LOW_THRESH,
	                                             params->EDGE_DETECTOR_HIGH_THRESH,
	                                             params->BINARIZE_THRESH,
	                                             params->BINARIZE_MAX_VALUE );

	calculator->lineDetector = sb::LineDetector( params->HOUGH_LINES_P_RHO,
	                                             params->HOUGH_LINES_P_THETA,
	                                             params->HOUGH_LINES_P_THRESHOLD,
	                                             params->HOUGH_LINES_P_MIN_LINE_LENGTH,
	                                             params->HOUGH_LINES_P_MAX_LINE_GAP );

	calculator->cropBox = params->CROP_BOX;

	std::vector<double> _splitRatio = { 0.2, 0.25, 0.25, 0.3 };

	calculator->splitBoxes.clear(); {
		int y = 0;
		for ( double ratio : _splitRatio ) {
			int h = static_cast<int>(round( 1.0 * params->CROP_BOX.height * ratio ));

			if ( y + h > params->CROP_BOX.height ) h = params->CROP_BOX.height - y;

			cv::Rect box( 0, y, params->CROP_BOX.width, h );
			calculator->splitBoxes.push_back( box );
			y += h;
		}
	}

	return 0;
}

int sb::calculate( sb::Calculator* calculator,
                   sb::RawContent* rawContent,
                   sb::FrameInfo* frameInfo )
{
	// 1) color image

	// crop to correct format
	if ( calculator->cropBox.x < 0 || calculator->cropBox.y < 0 ||
		calculator->cropBox.x + calculator->cropBox.width > rawContent->colorImage.cols ||
		calculator->cropBox.y + calculator->cropBox.height > rawContent->colorImage.rows ) {
		std::cerr << "Input image and crop box aren't suitable." << std::endl;
		return -1;
	}
	frameInfo->colorImage = rawContent->colorImage( calculator->cropBox );

	// flip to natural direction ( input image from camera was flipped )
	cv::flip( frameInfo->colorImage, frameInfo->colorImage, 1 );

	// 2) generate edges-frame
	cv::cvtColor( frameInfo->colorImage, frameInfo->edgesImage, cv::COLOR_BGR2GRAY );
	calculator->edgeDetector.apply( frameInfo->edgesImage );

	// 3) generate sections
	frameInfo->imageSections.clear();
	frameInfo->imageSections.reserve( 5 );
	for ( auto it = calculator->splitBoxes.crbegin(); it != calculator->splitBoxes.crend(); ++it ) {
		frameInfo->imageSections.push_back( sb::Section( frameInfo->edgesImage, *it ) );
	}

	// 4) generate lines for each section

	for ( auto it_section = frameInfo->imageSections.begin(); it_section != frameInfo->imageSections.end(); ++it_section ) {
		std::vector<sb::Line> lines;
		calculator->lineDetector.apply( it_section->getBinaryImage(), lines );

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

	return 0;
}

void sb::release( sb::Calculator* calculator ) {}
