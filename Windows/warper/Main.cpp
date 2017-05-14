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

	double maxViewWidth;
	double maxViewHeight;

	cv::String fileToSave;
};

int doModeGenerate( const int argc, const char** argv );

int doModeTest( const int argc, const char** argv );

int parseCommands( const int argc, const char** argv,
                   cv::Mat& image,
                   cv::String& filePath );

void saveData( WarpData* warpData );

void onMouseGeneratingWarpMatrix( int event, int x, int y, int flags, void* userdata );

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

	std::cout << "Enter max view width: ";
	std::cin >> warpData.maxViewWidth;

	warpData.maxViewHeight = 1.0 * warpData.image.rows / warpData.image.cols * warpData.maxViewWidth;
	std::cout << "Your max view height must be " << warpData.maxViewHeight << std::endl;

	cv::namedWindow( WINDOW_NAME, CV_WINDOW_KEEPRATIO );

	cv::imshow( WINDOW_NAME, warpData.image );

	cv::setMouseCallback( WINDOW_NAME, onMouseGeneratingWarpMatrix, &warpData );

	cv::waitKey();

	cv::setMouseCallback( WINDOW_NAME, NULL );

	saveData( &warpData );

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
	warpData->srcQuad[2].x = 0;
	warpData->srcQuad[3].x = static_cast<float>(warpData->image.cols - 1);

	warpData->dstQuad[0] = cv::Point2d( 0, 0 );
	warpData->dstQuad[1] = cv::Point2d( warpData->image.cols - 1, 0 );
	warpData->dstQuad[2] = cv::Point2d( 0, warpData->image.rows - 1 );
	warpData->dstQuad[3] = cv::Point2d( warpData->image.cols - 1, warpData->image.rows - 1 );

	// save to disk
	cv::FileStorage fs( warpData->fileToSave, cv::FileStorage::WRITE );
	fs << "WARP_SRC_QUAD" << std::vector<cv::Point2f>( warpData->srcQuad, warpData->srcQuad + 4 );
	fs << "WARP_DST_QUAD" << std::vector<cv::Point2f>( warpData->dstQuad, warpData->dstQuad + 4 );
	fs << "CONVERT_COEF" << warpData->maxViewWidth / warpData->image.cols;
	fs.release();
}

void onMouseGeneratingWarpMatrix( int event, int x, int y, int flags, void* userdata )
{
	WarpData* warpData = static_cast<WarpData*>(userdata);

	switch ( event ) {
	case cv::EVENT_LBUTTONDOWN: {

		if ( warpData->srcQuad[0].x < 0 || warpData->srcQuad[1].x < 0 ) {
			int center = warpData->image.cols / 2;
			int offsetX = abs( x - center );

			warpData->srcQuad[0].x = static_cast<float>(center - offsetX);
			warpData->srcQuad[1].x = static_cast<float>(center + offsetX);
		}
		else if ( warpData->srcQuad[0].y < 0 || warpData->srcQuad[1].y < 0 ) {
			warpData->srcQuad[0].y = static_cast<float>(y);
			warpData->srcQuad[1].y = static_cast<float>(y);
		}
		else if ( warpData->srcQuad[2].y < 0 || warpData->srcQuad[3].y < 0 ) {
			warpData->srcQuad[2].y = static_cast<float>(y);
			warpData->srcQuad[3].y = static_cast<float>(y);
		}

	}
		break;

	case cv::EVENT_MOUSEMOVE: {
		cv::Mat tempImage = warpData->image.clone();

		cv::putText( tempImage,
		             "Indicate your real width and height in the image",
		             cv::Point( 20, 15 ),
		             cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar( 0, 255, 255 ), 1 );

		if ( warpData->srcQuad[0].x < 0 || warpData->srcQuad[1].x < 0 ) {
			int center = warpData->image.cols / 2;
			int offsetX = abs( x - center );

			double x0 = static_cast<float>(center - offsetX);
			double x1 = static_cast<float>(center + offsetX);

			cv::line( tempImage,
			          cv::Point2d( x0, 0 ),
			          cv::Point2d( x0, warpData->image.rows - 1 ),
			          cv::Scalar( 0, 255, 0 ), 2 );
			cv::line( tempImage,
			          cv::Point2d( x1, 0 ),
			          cv::Point2d( x1, warpData->image.rows - 1 ),
			          cv::Scalar( 0, 255, 0 ), 2 );
		}
		else if ( warpData->srcQuad[0].y < 0 || warpData->srcQuad[1].y < 0 ) {
			cv::line( tempImage,
			          cv::Point2d( 0, y ),
			          cv::Point2d( warpData->image.cols - 1, y ),
			          cv::Scalar( 0, 255, 0 ), 2 );
		}
		else if ( warpData->srcQuad[2].y < 0 || warpData->srcQuad[3].y < 0 ) {
			cv::line( tempImage,
			          cv::Point2d( 0, y ),
			          cv::Point2d( warpData->image.cols - 1, y ),
			          cv::Scalar( 0, 255, 0 ), 2 );
		}

		cv::imshow( WINDOW_NAME, tempImage );
	}
		break;
	}
}
