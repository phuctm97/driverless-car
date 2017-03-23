#include "JoinerSplitter.h"

void sb::JoinerSplitter::splitImageToSections( const cv::Mat& image, std::vector<sb::Section>& sections ) const
{
	// safely clear the list
	sections.clear();

	// section info
	int sectionTopPosition = 0;

	int sectionIndex = 0;

	for ( float ratio : _ratios ) {

		// calculate section height based on ratio
		int sectionHeight = static_cast<int>(roundf( (1.0f * image.rows * ratio) ));

		// handle overflowed section
		if ( sectionTopPosition + sectionHeight > image.rows ) {
			sectionHeight = image.rows - sectionTopPosition;
		}

		// calculate section area based on width source image width
		cv::Rect2d sectionArea( 0, sectionTopPosition, image.cols, sectionHeight );

		// generate image section
		sections.push_back( sb::Section( sectionIndex, sectionArea, image ) );

		// translate to next section top position and increase index
		sectionTopPosition += sectionHeight;
		sectionIndex++;
	}
}

void sb::JoinerSplitter::joinSectionsToImage( const std::vector<sb::Section>& sections, cv::Mat& image )
{
	int nCols = sections.front().getDataImage().cols;
	int nRows = 0;
	for ( auto section : sections ) nRows += section.getDataImage().rows;

	image.create( cv::Size2d( nCols, nRows ), sections.front().getDataImage().type() );

	int sizeCounter = 0;
	for ( const sb::Section& section : sections ) {
		section.getDataImage().copyTo( image( cv::Rect2d( 0, sizeCounter, nCols, section.getDataImage().rows ) ) );

		sizeCounter += section.getDataImage().rows;
	}
}
