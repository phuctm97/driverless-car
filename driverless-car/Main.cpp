#include <opencv2/opencv.hpp>

void main()
{
	cv::VideoCapture webCam( 0 );

	cv::Mat frame;

	while ( true ) {
		webCam >> frame;

		cv::imshow( "Window", frame );

		if ( cv::waitKey( 33 ) == 27 )break;
	}
}
