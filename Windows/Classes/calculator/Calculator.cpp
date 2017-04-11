#include "Calculator.h"

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
	sb::clear( frameInfo );

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

	// 2) generate gray-scaled image
	cv::cvtColor( frameInfo->colorImage, frameInfo->edgesImage, cv::COLOR_BGR2GRAY );

	// 3) binarize image
	cv::threshold( frameInfo->edgesImage, frameInfo->binaryImage, 200, 255, cv::THRESH_BINARY );
	
	// 4) edges image
	calculator->edgeDetector.apply( frameInfo->edgesImage );

	// 5) generate sections
	frameInfo->imageSections.reserve( 5 );
	for ( auto it = calculator->splitBoxes.crbegin(); it != calculator->splitBoxes.crend(); ++it ) {
		sb::Section* section = new sb::Section;
		sb::create( section, frameInfo->binaryImage, frameInfo->edgesImage, *it );
		frameInfo->imageSections.push_back( section );

		cv::imshow( "Binary", section->binaryImage );
		cv::imshow( "Edges", section->edgesImage );
		cv::waitKey();
	}

	// 6) generate lines for each section

	for ( auto it_section = frameInfo->imageSections.begin(); it_section != frameInfo->imageSections.end(); ++it_section ) {
		std::vector<sb::Line> lines;
		calculator->lineDetector.apply( (*it_section)->edgesImage, lines );

		(*it_section)->imageLines.reserve( lines.size() );

		for ( auto it_line = lines.cbegin(); it_line != lines.cend(); ++it_line ) {
			sb::Line translatedLine( sb::convertToContainerSpace( *it_section, it_line->getStartingPoint() ),
			                         sb::convertToContainerSpace( *it_section, it_line->getEndingPoint() ) );

			sb::LineInfo* lineInfo = new sb::LineInfo;
			sb::create( lineInfo, translatedLine );

			cv::Point2d topPoint, bottomPoint;
			if ( !sb::Line::findIntersection( (*it_section)->topLine, translatedLine, topPoint ) ) continue;
			if ( !sb::Line::findIntersection( (*it_section)->bottomLine, translatedLine, bottomPoint ) ) continue;

			lineInfo->topPoint = topPoint;
			lineInfo->bottomPoint = bottomPoint;
			lineInfo->centerPoint = (topPoint + bottomPoint) * 0.5;

			(*it_section)->imageLines.push_back( lineInfo );
		}

		std::sort( (*it_section)->imageLines.begin(), (*it_section)->imageLines.end(),
		           [](const sb::LineInfo* l1, const sb::LineInfo* l2)-> bool {
			           return l1->bottomPoint.x < l2->bottomPoint.x;
		           } );
	}

	return 0;
}

void sb::release( sb::Calculator* calculator ) {}

void sb::findBlobs( const cv::Mat& binary, std::vector<std::vector<cv::Point2i>>& blobs )
{
	blobs.clear();
	cv::Mat labelImage;
	binary.convertTo( labelImage, CV_32SC1 );
	int labelCount = 2;
	for ( int y = 0; y < labelImage.rows; y++ ) {
		int* row = reinterpret_cast<int*>(labelImage.ptr( y ));
		for ( int x = 0; x < labelImage.cols; x++ ) {
			if ( row[x] != 255 )
				continue;
			cv::Rect rect;
			cv::floodFill( labelImage, cv::Point( x, y ), labelCount, &rect, 0, 0, 4 );
			std::vector<cv::Point2i> blob;
			for ( int i = rect.y; i < (rect.y + rect.height); i++ ) {
				int* row2 = reinterpret_cast<int*>(labelImage.ptr( i ));
				for ( int j = rect.x; j < (rect.x + rect.width); j++ ) {
					if ( row2[j] != labelCount )
						continue;
					blob.push_back( cv::Point( j, i ) );
				}

			}
			blobs.push_back( blob );
			labelCount++;
		}
	}
}
