#include "Section.h"

void sb::create( sb::Section* section,
								 const cv::Mat& containerBinaryImage,
								 const cv::Mat& containerEdgesImage,
								 const cv::Rect& rect )
{
	section->binaryImage = containerBinaryImage( rect );

	section->edgesImage = containerEdgesImage( rect );

	section->imageRect = rect;

	section->bottomLine = sb::Line( cv::Point2d( 0, rect.y + rect.height - 1 ),
	                                cv::Point2d( 1, rect.y + rect.height - 1 ) );
	section->topLine = sb::Line( cv::Point2d( 0, rect.y ),
	                             cv::Point2d( 1, rect.y ) );
}

void sb::clear( sb::Section* section )
{
	section->binaryImage.release();
	for( auto it_line = section->imageLines.begin(); it_line != section->imageLines.end(); ++it_line ) {
		delete *it_line;
	}
	section->imageLines.clear();
}

cv::Point2d sb::convertToContainerSpace( sb::Section* section, const cv::Point2d& pos )
{
	return cv::Point2d( section->imageRect.x + pos.x, pos.y + section->imageRect.y );
}
