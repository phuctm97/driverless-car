#include "Calculator.h"

int sb::init( sb::Calculator* calculator, sb::Params* params )
{
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

	// 2) binarize image
	cv::cvtColor( frameInfo->colorImage, frameInfo->binaryImage, cv::COLOR_BGR2GRAY );
	cv::threshold( frameInfo->binaryImage, frameInfo->binaryImage, 200, 255, cv::THRESH_BINARY );

	// 3) generate sections
	frameInfo->imageSections.reserve( 5 );
	for ( auto it = calculator->splitBoxes.crbegin(); it != calculator->splitBoxes.crend(); ++it ) {
		sb::Section* section = new sb::Section;
		sb::create( section, frameInfo->binaryImage, *it );
		frameInfo->imageSections.push_back( section );
	}

	// 4) generate blobs
	for ( auto it_section = frameInfo->imageSections.begin(); it_section != frameInfo->imageSections.end(); ++it_section ) {
		sb::Section* section = *it_section;
		sb::findBlobs( section->binaryImage, section->blobs );
	}

	return 0;
}

void sb::release( sb::Calculator* calculator ) {}

void sb::findBlobs( const cv::Mat& binary, std::vector<sb::Blob*>& blobs )
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
			sb::Blob* blob = new sb::Blob;
			blob->minX = INT_MAX;
			blob->maxX = 0;
			blob->minY = INT_MAX;
			blob->maxY = 0;
			for ( int i = rect.y; i < (rect.y + rect.height); i++ ) {
				int* row2 = reinterpret_cast<int*>(labelImage.ptr( i ));
				for ( int j = rect.x; j < (rect.x + rect.width); j++ ) {
					if ( row2[j] != labelCount )
						continue;
					blob->pixels.push_back( cv::Point( j, i ) );
					blob->minX = MIN( blob->minX, j );
					blob->maxX = MAX( blob->maxX, j );
					blob->minY = MIN( blob->minY, i );
					blob->maxY = MAX( blob->maxY, i );
				}

			}
			blobs.push_back( blob );
			labelCount++;
		}
	}
}
