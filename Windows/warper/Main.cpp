#include <opencv2/opencv.hpp>

#define WINDOW_NAME "Window"

struct WarpData
{
	cv::Mat image;

	cv::Point2f srcQuad[4] = {
		cv::Point2f( -1, -1 ),
		cv::Point2f( -1, -1 ),
		cv::Point2f( -1, -1 ),
		cv::Point2f( -1, -1 ) };

	cv::Point2f dstQuad[4] = {
		cv::Point2f( -1, -1 ),
		cv::Point2f( -1, -1 ),
		cv::Point2f( -1, -1 ),
		cv::Point2f( -1, -1 ) };

	cv::String fileToSave;
};

int doModeGenerate( const int argc, const char** argv );

int doModeTest( const int argc, const char** argv );

int parseCommands( const int argc, const char** argv,
                   cv::Mat& image,
                   cv::String& filePath );

void saveData( WarpData* warpData );

void onMouse( int event, int x, int y, int flags, void* userdata );

int main( const int argc, const char** argv )
{
	if ( argc < 2 ) return -1;

	if ( std::string( argv[1] ) == "g" ) {
		return doModeGenerate( argc, argv );
	}
	else if ( std::string( argv[1] ) == "t" ) {
		return doModeTest( argc, argv );
	}

	return 0;
}

int doModeGenerate( const int argc, const char** argv )
{
	WarpData warpData;

	if ( parseCommands( argc, argv, warpData.image, warpData.fileToSave ) < 0 )
		return -1;

	cv::namedWindow( WINDOW_NAME, CV_WINDOW_KEEPRATIO );

	cv::imshow( WINDOW_NAME, warpData.image );

	cv::setMouseCallback( WINDOW_NAME, onMouse, &warpData );

	cv::waitKey( 0 );

	saveData( &warpData );

	cv::setMouseCallback( WINDOW_NAME, NULL );

	return 0;
}

int doModeTest( const int argc, const char** argv )
{
	cv::Mat srcImage;
	
	cv::Mat dstImage;
	
	cv::String fileToLoad;

	// load source image and yaml
	if ( parseCommands( argc, argv, srcImage, fileToLoad ) < 0 )
		return -1;

	// warping
	{
		cv::Point2f srcQuad[4];
		cv::Point2f dstQuad[4];

		// load srcQuad and dstQuad
		std::vector<cv::Point2f> srcQuadVec;
		std::vector<cv::Point2f> dstQuadVec; 
		
		cv::FileStorage fs( fileToLoad, cv::FileStorage::READ );
		fs["WARP_SRC_QUAD"] >> srcQuadVec;
		fs["WARP_DST_QUAD"] >> dstQuadVec;
		fs.release();
		
		std::copy( srcQuadVec.begin(), srcQuadVec.end(), srcQuad );
		std::copy( dstQuadVec.begin(), dstQuadVec.end(), dstQuad );

		cv::Mat warpMat = cv::getPerspectiveTransform( srcQuad, dstQuad );		

		cv::warpPerspective( srcImage, dstImage, warpMat, srcImage.size() );
	}

	cv::imshow( "Source", srcImage );

	cv::imshow( "Destitation", dstImage );

	cv::waitKey();

	return 0;
}

int parseCommands( const int argc, const char** argv,
                   cv::Mat& image,
                   cv::String& filePath )
{
	if ( argc < 5 ) return -1;

	std::string type = argv[2];
	std::string src = argv[3];
	filePath = argv[4];

	if ( type == "i" ) {
		image = cv::imread( src );
	}
	else if ( type == "v" ) {
		cv::VideoCapture cap( src );
		cap >> image;
		cap.release();
	}

	return 0;
}

void saveData( WarpData* warpData )
{
	// swap the bottom left quad and bottom right quad
	{
		cv::Point2f temp;

		temp = warpData->srcQuad[2];
		warpData->srcQuad[2] = warpData->srcQuad[3];
		warpData->srcQuad[3] = temp;

		temp = warpData->dstQuad[2];
		warpData->dstQuad[2] = warpData->dstQuad[3];
		warpData->dstQuad[3] = temp;
	}

	// save to disk
	cv::FileStorage fs( warpData->fileToSave, cv::FileStorage::WRITE );
	fs << "WARP_SRC_QUAD" << std::vector<cv::Point2f>( warpData->srcQuad, warpData->srcQuad + 4 );
	fs << "WARP_DST_QUAD" << std::vector<cv::Point2f>( warpData->dstQuad, warpData->dstQuad + 4 );
	fs.release();
}

void onMouse( int event, int x, int y, int flags, void* userdata )
{
	WarpData* warpData = static_cast<WarpData*>(userdata);

	switch ( event ) {
	case cv::EVENT_LBUTTONDOWN: {

		for ( int i = 0; i < 4; i++ ) {
			if ( warpData->srcQuad[i].x < 0 ) {
				warpData->srcQuad[i] = cv::Point2f( static_cast<float>(x), static_cast<float>(y) );
				return;
			}
		}
		for ( int i = 0; i < 4; i++ ) {
			if ( warpData->dstQuad[i].x < 0 ) {
				warpData->dstQuad[i] = cv::Point2f( static_cast<float>(x), static_cast<float>(y) );
				return;
			}
		}
	}
		break;

	case cv::EVENT_MOUSEMOVE: {
		cv::Mat tempImage = warpData->image.clone();

		int i;
		for ( i = 0; i < 4; i++ ) {
			if ( warpData->srcQuad[i].x < 0 ) break;

			cv::circle( tempImage, warpData->srcQuad[i], 3, cv::Scalar( 0, 0, 255 ), -1 );

			if ( i > 0 ) {
				cv::line( tempImage,
				          warpData->srcQuad[i - 1],
				          warpData->srcQuad[i],
				          cv::Scalar( 0, 255, 0 ), 2 );
			}

			if ( i == 3 ) {
				cv::line( tempImage,
				          warpData->srcQuad[0],
				          warpData->srcQuad[3],
				          cv::Scalar( 0, 255, 0 ), 2 );
			}
		}

		if ( i > 0 && i < 4 ) {
			cv::line( tempImage,
			          warpData->srcQuad[i - 1],
			          cv::Point2f( static_cast<float>(x), static_cast<float>(y) ),
			          cv::Scalar( 0, 255, 0 ), 2 );
			if ( i == 3 ) {
				cv::line( tempImage,
				          warpData->srcQuad[0],
				          cv::Point2f( static_cast<float>(x), static_cast<float>(y) ),
				          cv::Scalar( 0, 255, 0 ), 2 );
			}
		}

		for ( i = 0; i < 4; i++ ) {
			if ( warpData->dstQuad[i].x < 0 ) break;

			cv::circle( tempImage, warpData->dstQuad[i], 3, cv::Scalar( 0, 0, 255 ), -1 );

			if ( i > 0 ) {
				cv::line( tempImage,
				          warpData->dstQuad[i - 1],
				          warpData->dstQuad[i],
				          cv::Scalar( 0, 255, 255 ), 2 );
			}

			if ( i == 3 ) {
				cv::line( tempImage,
				          warpData->dstQuad[0],
				          warpData->dstQuad[3],
				          cv::Scalar( 0, 255, 255 ), 2 );
			}
		}

		if ( i > 0 && i < 4 ) {
			cv::line( tempImage,
			          warpData->dstQuad[i - 1],
			          cv::Point2f( static_cast<float>(x), static_cast<float>(y) ),
			          cv::Scalar( 0, 255, 255 ), 2 );
			if ( i == 3 ) {
				cv::line( tempImage,
				          warpData->dstQuad[0],
				          cv::Point2f( static_cast<float>(x), static_cast<float>(y) ),
				          cv::Scalar( 0, 255, 255 ), 2 );
			}
		}

		cv::imshow( WINDOW_NAME, tempImage );
	}
		break;
	}
}
