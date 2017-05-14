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

	std::vector<double> splitRatio = { 0.2, 0.25, 0.25, 0.3 };

	calculator->splitBoxes.clear(); {
		int y = 0;
		for ( double ratio : splitRatio ) {
			int h = static_cast<int>(round( 1.0 * params->CROP_BOX.height * ratio ));

			if ( y + h > params->CROP_BOX.height ) h = params->CROP_BOX.height - y;

			cv::Rect box( 0, y, params->CROP_BOX.width, h );
			calculator->splitBoxes.push_back( box );
			y += h;
		}
	}

	std::reverse( calculator->splitBoxes.begin(), calculator->splitBoxes.end() );

	return 0;
}

int sb::calculate( sb::Calculator* calculator,
                   sb::RawContent* rawContent,
                   sb::FrameInfo* frameInfo )
{
	sb::release( frameInfo, false );

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

	// 4) generate blobs
	findBlobs( calculator, frameInfo );

	return 0;
}

void sb::release( sb::Calculator* calculator )
{
	calculator->splitBoxes.clear();
}

void sb::findBlobs( sb::Calculator* calculator, sb::FrameInfo* frameInfo )
{
	// generate binary short image
	cv::Mat labelImage;
	frameInfo->binImage.convertTo( labelImage, CV_32SC1 );

	// finding blob process
	int labelCount = 2;
	for ( int y = 0; y < labelImage.rows; y++ ) {
		int* row = reinterpret_cast<int*>(labelImage.ptr( y ));
		for ( int x = 0; x < labelImage.cols; x++ ) {
			if ( row[x] != 255 )
				continue;

			// find signal of a blob, floodfill to find the full one
			cv::Rect rect;
			cv::floodFill( labelImage, cv::Point( x, y ), labelCount, &rect, 0, 0, 4 );

			// allocate new blob
			sb::Blob* blob = new sb::Blob;
			blob->childBlobs.reserve( calculator->splitBoxes.size() );
			for ( auto cit_section = calculator->splitBoxes.cbegin(); cit_section != calculator->splitBoxes.cend(); ++cit_section ) {
				blob->childBlobs.push_back( new Blob );
			}

			// <minX, minY, maxX, maxY, sumX, sumY, size>
			std::tuple<int, int, int, int, long, long, size_t> blobInfo = std::make_tuple( INT_MAX, INT_MAX, 0, 0, 0, 0, 0 );
			std::vector<std::tuple<int, int, int, int, long, long, size_t>> childBlobsInfo;
			childBlobsInfo.assign( blob->childBlobs.size(), std::make_tuple( INT_MAX, INT_MAX, 0, 0, 0, 0, 0 ) );

			// find pixels in blob
			for ( int i = rect.y; i < (rect.y + rect.height); i++ ) {
				int* row2 = reinterpret_cast<int*>(labelImage.ptr( i ));
				for ( int j = rect.x; j < (rect.x + rect.width); j++ ) {
					if ( row2[j] != labelCount )
						continue;

					// update main blob info
					std::get<0>( blobInfo ) = MIN( std::get<0>( blobInfo ), j );
					std::get<1>( blobInfo ) = MIN( std::get<1>( blobInfo ), i );
					std::get<2>( blobInfo ) = MAX( std::get<2>( blobInfo ), j );
					std::get<3>( blobInfo ) = MAX( std::get<3>( blobInfo ), i );
					std::get<4>( blobInfo ) += j;
					std::get<5>( blobInfo ) += i;
					std::get<6>( blobInfo )++;

					// update child blobs info
					auto cit_section = calculator->splitBoxes.cbegin();
					auto it_blobinfo = childBlobsInfo.begin();
					for ( ; it_blobinfo != childBlobsInfo.end(); ++it_blobinfo , ++cit_section ) {
						if ( i >= cit_section->y ) {
							std::get<0>( *it_blobinfo ) = MIN( std::get<0>( *it_blobinfo ), j );
							std::get<1>( *it_blobinfo ) = MIN( std::get<1>( *it_blobinfo ), i );
							std::get<2>( *it_blobinfo ) = MAX( std::get<2>( *it_blobinfo ), j );
							std::get<3>( *it_blobinfo ) = MAX( std::get<3>( *it_blobinfo ), i );
							std::get<4>( *it_blobinfo ) += j;
							std::get<5>( *it_blobinfo ) += i;
							std::get<6>( *it_blobinfo )++;
							break;
						}
					}
				}
			}

			if ( std::get<6>( blobInfo ) >= MIN_ACCEPTABLE_BLOB_OBJECTS_COUNT ) {
				blob->box = cv::Rect( std::get<0>( blobInfo ), std::get<1>( blobInfo ),
				                      std::get<2>( blobInfo ) - std::get<0>( blobInfo ) + 1,
				                      std::get<3>( blobInfo ) - std::get<1>( blobInfo ) + 1 );
				blob->size = std::get<6>( blobInfo );
				blob->origin = cv::Point( static_cast<int>(std::get<4>( blobInfo ) / blob->size),
				                          static_cast<int>(std::get<5>( blobInfo ) / blob->size) );

				auto cit_info = childBlobsInfo.cbegin();
				auto cit_childblob = blob->childBlobs.cbegin();
				for ( ; cit_info != childBlobsInfo.cend(); ++cit_info , ++cit_childblob ) {

					// TODO: MIN_ACCEPTABLE_CHILD_BLOB_OBJECTS_COUNT
					if ( std::get<6>( *cit_info ) <= 5 ) continue;

					Blob* childBlob = *cit_childblob;
					childBlob->box = cv::Rect( std::get<0>( *cit_info ), std::get<1>( *cit_info ),
					                           std::get<2>( *cit_info ) - std::get<0>( *cit_info ) + 1,
					                           std::get<3>( *cit_info ) - std::get<1>( *cit_info ) + 1 );
					childBlob->size = std::get<6>( *cit_info );
					childBlob->origin = cv::Point( static_cast<int>(std::get<4>( *cit_info ) / childBlob->size),
					                               static_cast<int>(std::get<5>( *cit_info ) / childBlob->size) );

				}
				frameInfo->blobs.push_back( blob );
			}
			else {
				sb::release( blob );
				delete blob;
			}
			labelCount++;
		}

	}
}

