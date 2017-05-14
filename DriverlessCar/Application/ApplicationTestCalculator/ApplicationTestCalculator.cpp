#include "ApplicationTestCalculator.h"
#include <ctime>

void sb::ApplicationTestCalculator::run()
{
	// init
	_exiting = false;

	sb::CollectData* collectData = new CollectData;
	sb::CalculateData* calculateData = new CalculateData;

	if( _collector == nullptr || _collector->init() < 0 ) _exiting = true;
	if( _calculator == nullptr || _calculator->init() < 0 ) _exiting = true;

	// run
	clock_t t;
	int frameCount = 0;

	while( !_exiting ) {
		std::cout << frameCount++ << std::endl;

		t = clock();
		if( _collector->collect( collectData ) < 0 ) break;
		std::cout << "Collector: " << 1000 * (clock() - t) / CLOCKS_PER_SEC << std::endl;

		cv::imshow( "Original image", collectData->colorImage );

		t = clock();
		if( _calculator->calculate( collectData, calculateData ) ) break;
		std::cout << "Calculator: " << 1000 * (clock() - t) / CLOCKS_PER_SEC << std::endl;

		showResult( calculateData );

		// keyboard event
		if( _keyboard != nullptr ) {
			_keyboard->checkBufferedKey();
		}
	}

	// release
	if( collectData != nullptr ) {
		sb::release( collectData );
		delete collectData;
	}

	if( calculateData != nullptr ) {
		sb::release( calculateData );
		delete calculateData;
	}
}

void sb::ApplicationTestCalculator::exit()
{
	_exiting = true;
}

void sb::ApplicationTestCalculator::release()
{
	if( _keyboard != nullptr ) {
		_keyboard->release();
		delete _keyboard;
	}

	if( _collector != nullptr ) {
		_collector->release();
		delete _collector;
	}

	if( _calculator != nullptr ) {
		_calculator->release();
		delete _calculator;
	}
}

void sb::ApplicationTestCalculator::addKeyboardCallback( const std::function<void( int )>& callback )
{
	if( _keyboard != nullptr ) _keyboard->addKeyboardCallback( callback );
}

void sb::ApplicationTestCalculator::showResult( sb::CalculateData* calculateData )
{
	cv::imshow( "BGR image", calculateData->bgrImage );

	cv::imshow( "BIN image", calculateData->binImage );

	for( auto cit_blob = calculateData->blobs.cbegin(); cit_blob != calculateData->blobs.cend(); ++cit_blob ) {
		cv::Mat img0 = calculateData->bgrImage.clone();
		cv::Mat img1; cv::cvtColor( calculateData->binImage, img1, cv::COLOR_GRAY2BGR );

		sb::Blob* blob = *cit_blob;
		for( auto cit_childblob = blob->childBlobs.cbegin(); cit_childblob != blob->childBlobs.cend(); ++cit_childblob ) {
			sb::Blob* childBlob = *cit_childblob;

			if( childBlob->size == 0 ) continue;

			cv::rectangle( img0, childBlob->box.tl(), childBlob->box.br(), cv::Scalar( 0, 0, 255 ), 1 );
			cv::rectangle( img1, childBlob->box.tl(), childBlob->box.br(), cv::Scalar( 0, 0, 255 ), 1 );
			cv::circle( img0, childBlob->origin, 3, cv::Scalar( 0, 255, 0 ), 2 );
			cv::circle( img1, childBlob->origin, 3, cv::Scalar( 0, 255, 0 ), 2 );
		}

		cv::putText( img0, std::to_string( blob->size ), cv::Point( calculateData->bgrImage.cols / 2, 30 ),
								 cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar( 0, 255, 255 ), 2 );
		cv::putText( img1, std::to_string( blob->size ), cv::Point( calculateData->bgrImage.cols / 2, 30 ),
								 cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar( 0, 255, 255 ), 2 );
		cv::imshow( "BGR image", img0 );
		cv::imshow( "BIN image", img1 );
		cv::waitKey();
	}

	/*for( auto cit_blob = calculateData->blobs.cbegin(); cit_blob != calculateData->blobs.cend(); ++cit_blob ) {

		Blob* blob = *cit_blob;

		for( auto cit_row = blob->rows.cbegin(); cit_row != blob->rows.cend(); ++cit_row ) {
			cv::Mat img0 = calculateData->bgrImage.clone();
			cv::Mat img1;
			cv::cvtColor( calculateData->binImage, img1, cv::COLOR_GRAY2BGR );
			cv::line( img0, cv::Point( cit_row->minX, cit_row->row ), cv::Point( cit_row->maxX, cit_row->row ), cv::Scalar( 0, 0, 255 ), 2 );
			cv::line( img1, cv::Point( cit_row->minX, cit_row->row ), cv::Point( cit_row->maxX, cit_row->row ), cv::Scalar( 0, 0, 255 ), 2 );
			cv::putText( img0, std::to_string( cit_row->width ), cv::Point( calculateData->bgrImage.cols / 2, 30 ),
									 cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar( 0, 255, 255 ), 2 );
			cv::putText( img1, std::to_string( cit_row->width ), cv::Point( calculateData->bgrImage.cols / 2, 30 ),
									 cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar( 0, 255, 255 ), 2 );
			cv::imshow( "BGR image", img0 );
			cv::imshow( "BIN image", img1 );
			cv::waitKey( 100 );
		}
	}*/
}
