#include "BlobTool.h"

sb::BlobTool::BlobTool( const std::vector<double>& splitRatio, const cv::Size& imageSize )
{
	_splitBoxes.clear();
	{
		int y = 0;
		for( double ratio : splitRatio ) {
			int h = static_cast<int>(round( 1.0 * imageSize.height * ratio ));

			if( y + h > imageSize.height ) h = imageSize.height - y;

			cv::Rect box( 0, y, imageSize.width, h );
			_splitBoxes.push_back( box );
			y += h;
		}
	}

	std::reverse( _splitBoxes.begin(), _splitBoxes.end() );
}

int sb::BlobTool::findBlobs( const cv::Mat& binImage, std::vector<sb::Blob*>& blobs )
{
	// generate binary short image
	cv::Mat labelImage;
	binImage.convertTo( labelImage, CV_32SC1 );

	// finding blob process
	int labelCount = 2;
	for( int y = 0; y < labelImage.rows; y++ ) {
		int* row = reinterpret_cast<int*>(labelImage.ptr( y ));
		for( int x = 0; x < labelImage.cols; x++ ) {
			if( row[x] != 255 )
				continue;

			// find signal of a blob, floodfill to find the full one
			cv::Rect rect;
			cv::floodFill( labelImage, cv::Point( x, y ), labelCount, &rect, 0, 0, 4 );

			// allocate new blob
			sb::Blob* blob = new sb::Blob;
			blob->childBlobs.reserve( _splitBoxes.size() );
			for( auto cit_section = _splitBoxes.cbegin(); cit_section != _splitBoxes.cend(); ++cit_section ) {
				blob->childBlobs.push_back( new Blob );
			}

			// <minX, minY, maxX, maxY, sumX, sumY, size>
			std::tuple<int, int, int, int, long, long, size_t> blobInfo = std::make_tuple( INT_MAX, INT_MAX, 0, 0, 0, 0, 0 );
			std::vector<std::tuple<int, int, int, int, long, long, size_t>> childBlobsInfo;
			childBlobsInfo.assign( blob->childBlobs.size(), std::make_tuple( INT_MAX, INT_MAX, 0, 0, 0, 0, 0 ) );

			// find pixels in blob
			for( int i = rect.y; i < (rect.y + rect.height); i++ ) {
				int* row2 = reinterpret_cast<int*>(labelImage.ptr( i ));
				BlobRow blobRow;
				blobRow.row = i;

				for( int j = rect.x; j < (rect.x + rect.width); j++ ) {
					if( row2[j] != labelCount )
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
					auto cit_section = _splitBoxes.cbegin();
					auto it_blobinfo = childBlobsInfo.begin();
					for( ; it_blobinfo != childBlobsInfo.end(); ++it_blobinfo, ++cit_section ) {
						if( i >= cit_section->y ) {
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

					// calculate width
					if( blobRow.minX < 0 )  blobRow.minX = j;
					blobRow.minX = MIN( blobRow.minX, j );

					if( blobRow.maxX < 0 ) blobRow.maxX = j;
					blobRow.maxX = MAX( blobRow.maxX, j );
				}

				if( blobRow.minX >= 0 && blobRow.maxX >= 0 ) {
					blobRow.width = blobRow.maxX - blobRow.minX + 1;
					blob->rows.push_back( blobRow );
				}
			}

			if( std::get<6>( blobInfo ) >= MIN_ACCEPTABLE_BLOB_OBJECTS_COUNT ) {
				blob->box = cv::Rect( std::get<0>( blobInfo ), std::get<1>( blobInfo ),
															std::get<2>( blobInfo ) - std::get<0>( blobInfo ) + 1,
															std::get<3>( blobInfo ) - std::get<1>( blobInfo ) + 1 );
				blob->size = std::get<6>( blobInfo );
				blob->origin = cv::Point( static_cast<int>(std::get<4>( blobInfo ) / blob->size),
																	static_cast<int>(std::get<5>( blobInfo ) / blob->size) );
				std::reverse( blob->rows.begin(), blob->rows.end() );

				auto cit_info = childBlobsInfo.cbegin();
				auto cit_childblob = blob->childBlobs.cbegin();
				for( ; cit_info != childBlobsInfo.cend(); ++cit_info, ++cit_childblob ) {

					// TODO: MIN_ACCEPTABLE_CHILD_BLOB_OBJECTS_COUNT
					if( std::get<6>( *cit_info ) <= 5 ) continue;

					Blob* childBlob = *cit_childblob;
					childBlob->box = cv::Rect( std::get<0>( *cit_info ), std::get<1>( *cit_info ),
																		 std::get<2>( *cit_info ) - std::get<0>( *cit_info ) + 1,
																		 std::get<3>( *cit_info ) - std::get<1>( *cit_info ) + 1 );
					childBlob->size = std::get<6>( *cit_info );
					childBlob->origin = cv::Point( static_cast<int>(std::get<4>( *cit_info ) / childBlob->size),
																				 static_cast<int>(std::get<5>( *cit_info ) / childBlob->size) );

				}
				blobs.push_back( blob );
			}
			else {
				sb::release( blob );
				delete blob;
			}
			labelCount++;
		}

	}

	return 0;
}