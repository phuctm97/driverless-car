#include "Calculator.h"

int sb::init( sb::Calculator* calculator, sb::Params* params )
{
	calculator->edgeDetector = sb::EdgeDetector( params->EDGE_DETECTOR_KERNEL_SIZE,
	                                             params->EDGE_DETECTOR_LOW_THRESH,
	                                             params->EDGE_DETECTOR_HIGH_THRESH,
	                                             params->BINARIZE_THRESH,
	                                             params->BINARIZE_MAX_VALUE );

	calculator->cropBox = params->CROP_BOX;

	calculator->binarizeThesh = params->BINARIZE_THRESH;

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
	sb::release( frameInfo );

	// TODO: remove images in section for optimization
	// TODO: generated local shared hsv image to calculate color for all blobs

	// 1) bgr image
	// crop to correct format
	if ( calculator->cropBox.x < 0 || calculator->cropBox.y < 0 ||
		calculator->cropBox.x + calculator->cropBox.width > rawContent->colorImage.cols ||
		calculator->cropBox.y + calculator->cropBox.height > rawContent->colorImage.rows ) {
		std::cerr << "Input image and crop box aren't suitable." << std::endl;
		return -1;
	}
	frameInfo->bgrImage = rawContent->colorImage( calculator->cropBox );
	// flip to natural direction ( input image from camera was flipped )
	cv::flip( frameInfo->bgrImage, frameInfo->bgrImage, 1 );

	// 2) binarize image
	cv::cvtColor( frameInfo->bgrImage, frameInfo->edgImage, cv::COLOR_BGR2GRAY );
	cv::threshold( frameInfo->edgImage, frameInfo->binImage, calculator->binarizeThesh, 255, cv::THRESH_BINARY );

	// 3) edges detector
	calculator->edgeDetector.apply( frameInfo->edgImage );

	// 4) generate sections
	frameInfo->imageSections.reserve( 5 );
	for ( auto it_box = calculator->splitBoxes.crbegin(); it_box != calculator->splitBoxes.crend(); ++it_box ) {
		sb::Section* section = new sb::Section;
		sb::create( section, frameInfo->bgrImage, frameInfo->binImage, frameInfo->edgImage, *it_box );
		frameInfo->imageSections.push_back( section );
	}

	// 5) generate blobs
	for ( auto it_section = frameInfo->imageSections.begin(); it_section != frameInfo->imageSections.end(); ++it_section ) {
		sb::Section* section = *it_section;
		sb::findBlobsInSection( section );

		std::sort( section->blobs.begin(), section->blobs.end(),
		           [](const sb::Blob* b1, const sb::Blob* b2)-> bool {
			           return b1->box.tl().x < b2->box.tl().x;
		           } );

		// TODO: calculate blob shape, pos, angle,...
	}

	return 0;
}

void sb::release( sb::Calculator* calculator )
{
	calculator->splitBoxes.clear();
}

void sb::findBlobsInSection( sb::Section* section )
{
	cv::Mat labelImage;
	section->binImage.convertTo( labelImage, CV_32SC1 );

	int minX, minY, maxX, maxY;
	long sumX, sumY;

	int labelCount = 2;

	for ( int y = 0; y < labelImage.rows; y++ ) {
		int* row = reinterpret_cast<int*>(labelImage.ptr( y ));
		for ( int x = 0; x < labelImage.cols; x++ ) {
			if ( row[x] != 255 )
				continue;
			cv::Rect rect;
			cv::floodFill( labelImage, cv::Point( x, y ), labelCount, &rect, 0, 0, 4 );

			sb::Blob* blob = new sb::Blob;
			minX = INT_MAX;
			minY = INT_MAX;
			maxX = 0;
			maxY = 0;
			sumX = 0;
			sumY = 0;

			int dominantHue = 0;

			int hueCounter[360]{ 0 };
			float sSum = 0.0;
			float vSum = 0.0;

			for ( int i = rect.y; i < (rect.y + rect.height); i++ ) {
				int* row2 = reinterpret_cast<int*>(labelImage.ptr( i ));
				for ( int j = rect.x; j < (rect.x + rect.width); j++ ) {
					if ( row2[j] != labelCount )
						continue;
					cv::Vec3b pixel = section->bgrImage.at<cv::Vec3b>( cv::Point( j, i ) );
					cv::Vec3f hsvPixel = cvtBGRtoHSV( pixel );

					int hValue = static_cast<int>(hsvPixel[0]);

					hueCounter[hValue]++;
					sSum += hsvPixel[1];
					vSum += hsvPixel[2];

					if ( hueCounter[hValue] > hueCounter[dominantHue] ) {
						dominantHue = hValue;
					}

					cv::Point pos = sb::convertToContainerSpace( section, cv::Point( j, i ) );
					blob->pixels.push_back( pos );
					minX = MIN( minX, pos.x );
					maxX = MAX( maxX, pos.x );
					minY = MIN( minY, pos.y );
					maxY = MAX( maxY, pos.y );
					sumX += pos.x;
					sumY += pos.y;
				}

			}
			if ( blob->pixels.size() >= MIN_ACCEPTABLE_BLOB_OBJECTS_COUNT ) {
				cv::Vec3f dominantHSV = cv::Vec3f( 1.0f * dominantHue, sSum / blob->pixels.size(), vSum / blob->pixels.size() );

				blob->bgr = cvtHSVtoBGR( dominantHSV );
				blob->box = cv::Rect( minX, minY, maxX - minX + 1, maxY - minY + 1 );
				blob->origin = cv::Point( static_cast<int>(sumX / blob->pixels.size()),
				                          static_cast<int>(sumY / blob->pixels.size()) );
				section->blobs.push_back( blob );
			}
			else {
				delete blob;
			}
			labelCount++;
		}

	}
}

cv::Vec3b sb::cvtHSVtoBGR( const cv::Vec3f& hsv )
{
	float h = hsv[0];
	float s = hsv[1];
	float v = hsv[2];

	float c = v * s;
	float temp = h / 60;
	temp = fmod( temp, 2.0f );
	temp -= 1;
	temp = fabs( temp );
	temp = 1 - temp;
	float x = temp * c;

	float m = v - c;

	float r;
	float g;
	float b;

	if ( h >= 0 && h < 60 ) {
		r = c;
		g = x;
		b = 0;
	}
	else if ( h >= 60 && h < 120 ) {
		r = x;
		g = c;
		b = 0;
	}
	else if ( h >= 120 && h < 180 ) {
		r = 0;
		g = c;
		b = x;
	}
	else if ( h >= 180 && h < 240 ) {
		r = 0;
		g = x;
		b = c;
	}
	else if ( h >= 240 && h < 300 ) {
		r = x;
		g = 0;
		b = c;
	}
	else {
		r = c;
		g = 0;
		b = x;
	}

	r = (r + m) * 255;
	g = (g + m) * 255;
	b = (b + m) * 255;

	return cv::Vec3b( static_cast<int>(b), static_cast<int>(g), static_cast<int>(r) );
}

cv::Vec3f sb::cvtBGRtoHSV( const cv::Vec3b& bgr )
{
	int b = bgr[0];
	int g = bgr[1];
	int r = bgr[2];

	int cMax = MAX( b, MAX( g, r ) );
	int cMin = MIN( b, MIN( g, r ) );

	int delta = cMax - cMin;

	float hue;
	float saturation;
	float value;

	if ( delta == 0 ) {
		hue = 0;
	}
	else {
		if ( cMax == r ) {
			hue = (g - b) / static_cast<float>(delta);
			//hue /= 255.0;
			hue = fmod( hue, 6.0f );
			hue *= 60;
		}
		else if ( cMax == g ) {
			hue = (b - r) / static_cast<float>(delta);
			//hue /= 255.0;
			hue += 2;
			hue *= 60;
		}
		else {
			hue = (r - g) / static_cast<float>(delta);
			//hue /= 255.0;
			hue += 4;
			hue *= 60;
		}
	}

	if ( cMax == 0 ) {
		saturation = 0;
	}
	else {
		saturation = delta / static_cast<float>(cMax);
	}

	value = cMax / 255.0f;

	if ( hue < 0 ) {
		hue += 360;
	}

	return cv::Vec3f( hue, saturation, value );
}
