#include <opencv2/opencv.hpp>

int main()
{
	cv::VideoCapture webCam( 0 );

	cv::Mat image;

	while( true ) {

		webCam >> image;

		cv::imshow( "Webcam", image );

		if( cv::waitKey( 33 ) == 27 )break;
	}

	return 0;
}