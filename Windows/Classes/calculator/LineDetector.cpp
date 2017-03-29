#include "LineDetector.h"

void sb::LineDetector::apply( const cv::Mat& binaryImage,
                              std::vector<sb::Line>& lines ) const
{
	// reset array
	lines.clear();

	// apply Hough transform
	std::vector<cv::Vec4i> houghLinesPOutput;
	cv::HoughLinesP( binaryImage,
	                 houghLinesPOutput,
	                 _houghLinesPRho,
	                 _houghLinesPTheta,
	                 _houghLinesPThreshold,
	                 _houghLinesPMinLineLength,
	                 _houghLinesPMaxLineGap );

	// generate sb::Line and push to the array
	for ( auto vec : houghLinesPOutput ) {

		sb::Line line( cv::Point2d( vec[0], vec[1] ),
		               cv::Point2d( vec[2], vec[3] ) );

		if ( !line.isValid() ) continue;

		lines.push_back( line );

	}
}

void sb::LineDetector::apply( const cv::Mat& binaryImage, 
															const cv::Mat& colorImage,
															std::vector<sb::LineInfo>& lines ) const
{
	// reset array
	lines.clear();

	// apply Hough transform
	std::vector<cv::Vec4i> houghLinesPOutput;
	cv::HoughLinesP( binaryImage,
	                 houghLinesPOutput,
	                 _houghLinesPRho,
	                 _houghLinesPTheta,
	                 _houghLinesPThreshold,
	                 _houghLinesPMinLineLength,
	                 _houghLinesPMaxLineGap );

	// generate sb::Line and push to the array
	for ( auto vec : houghLinesPOutput ) {

		sb::Line line( cv::Point2d( vec[0], vec[1] ),
		               cv::Point2d( vec[2], vec[3] ) );

		if ( !line.isValid() ) continue;

		lines.push_back( LineInfo( colorImage, line ) );

	}
}
