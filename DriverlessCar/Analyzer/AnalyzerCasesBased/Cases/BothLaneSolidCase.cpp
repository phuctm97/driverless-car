#include "BothLaneSolidCase.h"
#include "../CaseRepository.h"

const cv::Point& sb::BothLaneSolidCase::getLeftLaneOrigin() const { return _leftLaneOrigin; }

const cv::Point& sb::BothLaneSolidCase::getRightLaneOrigin() const { return _rightLaneOrigin; }

const int& sb::BothLaneSolidCase::getLeftLaneSize() const { return _leftLaneSize; }

const int& sb::BothLaneSolidCase::getRightLaneSize() const { return _rightLaneSize; }

const int& sb::BothLaneSolidCase::getLeftLaneHeight() const { return _leftLaneHeight; }

const int& sb::BothLaneSolidCase::getRightLaneHeight() const { return _rightLaneHeight; }

const std::vector<sb::BlobRow>& sb::BothLaneSolidCase::getLeftRows() const { return _leftRows; }

const std::vector<sb::BlobRow>& sb::BothLaneSolidCase::getRightRows() const { return _rightRows; }

const std::vector<std::pair<int, int>>& sb::BothLaneSolidCase::getLeftGoodSections() const { return _leftGoodSections; }

const std::vector<std::pair<int, int>>& sb::BothLaneSolidCase::getLeftBadSections() const { return _leftBadSections; }

const std::vector<std::pair<int, int>>& sb::BothLaneSolidCase::getRightGoodSections() const { return _rightGoodSections; }

const std::vector<std::pair<int, int>>& sb::BothLaneSolidCase::getRightBadSections() const { return _rightBadSections; }

const int& sb::BothLaneSolidCase::getObstaclePosition() const { return _obstaclePosition; }

int sb::BothLaneSolidCase::getType()
{
	return CaseType::BOTH_LANE_SOLID_CASE;
}

int sb::BothLaneSolidCase::analyze( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, AnalyzeData* analyzeData )
{
	// first time run
	if ( caseRepository->empty() ) {
		return firstAnalyze( caseRepository, collectData, calculateData, analyzeData );
	}

	return trackAnalyze( caseRepository, collectData, calculateData, analyzeData );
}

int sb::BothLaneSolidCase::firstAnalyze( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, AnalyzeData* analyzeData )
{
	// find 2 largest blob
	Blob* largestBlobs[2] = { nullptr };
	if ( findTwoLargestBlobs( collectData, calculateData, largestBlobs ) < 0 )
		return -1;

	// set left blob and right blob
	Blob* leftBlob = largestBlobs[0]->origin.x < largestBlobs[1]->origin.x ? largestBlobs[0] : largestBlobs[1];
	Blob* rightBlob = largestBlobs[0]->origin.x > largestBlobs[1]->origin.x ? largestBlobs[0] : largestBlobs[1];

	// check for size
	if ( leftBlob->size < _params->MIN_LANE_BLOB_SIZE || rightBlob->size < _params->MIN_LANE_BLOB_SIZE ) {
		std::cerr << "BothLaneSolidCase: init lane size too small" << std::endl;
		return -1;
	}

	// check for height
	if ( leftBlob->box.height < _params->MIN_LANE_BLOB_HEIGHT || rightBlob->box.height < _params->MIN_LANE_BLOB_HEIGHT ) {
		std::cerr << "BothLaneSolidCase: init lane size too short" << std::endl;
		return -1;
	}

	// store case's analyze data to repository
	auto caseToSave = new BothLaneSolidCase( _params, _obstacleFinder );
	caseRepository->push( caseToSave );

	// check for left lane row width
	caseToSave->_leftGoodSections.clear();
	caseToSave->_leftBadSections.clear(); {
		auto it_row = leftBlob->rows.begin();
		while ( it_row != leftBlob->rows.end() ) {
			auto pre_row = it_row;

			if ( (it_row->row >= calculateData->bgrImage.rows / 2 && (it_row->width < _params->MIN_LANE_WIDTH_1 || it_row->width > _params->MAX_LANE_WIDTH_1))
				|| (it_row->row < calculateData->bgrImage.rows / 2 && (it_row->width < _params->MIN_LANE_WIDTH_2 || it_row->width > _params->MAX_LANE_WIDTH_2)) ) {
				beginBadSection( it_row, leftBlob, calculateData );
				caseToSave->_leftBadSections.push_back( std::make_pair( static_cast<int>(std::distance( leftBlob->rows.begin(), pre_row )), static_cast<int>(std::distance( pre_row, it_row )) ) );
			}
			else {
				beginCorrectSection( it_row, leftBlob, calculateData );
				caseToSave->_leftGoodSections.push_back( std::make_pair( static_cast<int>(std::distance( leftBlob->rows.begin(), pre_row )), static_cast<int>(std::distance( pre_row, it_row )) ) );
			}
		}
	}

	// check for right lane width
	caseToSave->_rightGoodSections.clear();
	caseToSave->_rightBadSections.clear(); {
		auto it_row = rightBlob->rows.begin();
		while ( it_row != rightBlob->rows.end() ) {
			auto pre_row = it_row;

			if ( (it_row->row >= calculateData->bgrImage.rows / 2 && (it_row->width < _params->MIN_LANE_WIDTH_1 || it_row->width > _params->MAX_LANE_WIDTH_1))
				|| (it_row->row < calculateData->bgrImage.rows / 2 && (it_row->width < _params->MIN_LANE_WIDTH_2 || it_row->width > _params->MAX_LANE_WIDTH_2)) ) {
				beginBadSection( it_row, rightBlob, calculateData );
				caseToSave->_rightBadSections.push_back( std::make_pair( static_cast<int>(std::distance( rightBlob->rows.begin(), pre_row )), static_cast<int>(std::distance( pre_row, it_row )) ) );
			}
			else {
				beginCorrectSection( it_row, rightBlob, calculateData );
				caseToSave->_rightGoodSections.push_back( std::make_pair( static_cast<int>(std::distance( rightBlob->rows.begin(), pre_row )), static_cast<int>(std::distance( pre_row, it_row )) ) );
			}
		}
	}

	// check good section percentage
	float leftGoodRatio = 0;
	float rightGoodRatio = 0; {
		int sum;

		sum = 0;
		for ( auto sectionInfo : caseToSave->_leftGoodSections ) sum += sectionInfo.second;
		leftGoodRatio = 1.0f * sum / calculateData->bgrImage.rows;

		sum = 0;
		for ( auto sectionInfo : caseToSave->_rightGoodSections ) sum += sectionInfo.second;
		rightGoodRatio = 1.0f * sum / calculateData->bgrImage.rows;
	}
	if ( leftGoodRatio < 0.5f && rightGoodRatio < 0.5f ) {
		std::cerr << "BothLaneSolidCase: bad init lanes" << std::endl;
		return -1; // TODO: dashed lane
	}

	// save size, height, shape: save to this, and push this to repository
	caseToSave->_leftLaneOrigin = leftBlob->origin;
	caseToSave->_rightLaneOrigin = rightBlob->origin;
	caseToSave->_leftLaneSize = static_cast<int>(leftBlob->size);
	caseToSave->_rightLaneSize = static_cast<int>(rightBlob->size);
	caseToSave->_leftLaneHeight = leftBlob->box.height;
	caseToSave->_rightLaneHeight = rightBlob->box.height;
	caseToSave->_leftRows = leftBlob->rows;
	caseToSave->_rightRows = rightBlob->rows;

	// calculate road width: save to repository in a map (from row index to road width)
	{
		auto cit_lrow = leftBlob->rows.cbegin();
		auto cit_rrow = rightBlob->rows.cbegin();
		while ( cit_lrow->row > cit_rrow->row && cit_lrow != leftBlob->rows.cend() ) ++cit_lrow;
		if ( cit_lrow == leftBlob->rows.cend() ) return -1;

		while ( cit_rrow->row > cit_lrow->row && cit_rrow != rightBlob->rows.cend() ) ++cit_rrow;
		if ( cit_rrow == rightBlob->rows.cend() ) return -1;

		while ( cit_lrow != leftBlob->rows.cend() && cit_rrow != rightBlob->rows.cend() ) {
			if ( cit_lrow->tag >= 0 && cit_rrow->tag >= 0 )
				caseRepository->mapRoadWidth( cit_lrow->row, cit_rrow->minX - cit_lrow->maxX + 1 );
			++cit_lrow;
			++cit_rrow;
		}
	}

	// debug
	/*{
		cv::Mat img = calculateData->bgrImage.clone();
		for ( auto row : caseToSave->_leftRows ) {
			cv::Scalar color;
			if ( row.tag == 0 ) color = cv::Scalar( 0, 255, 0 );
			else if ( row.tag == 1 ) color = cv::Scalar( 0, 255, 255 );
			else if ( row.tag == -1 ) color = cv::Scalar( 0, 0, 255 );

			cv::line( img, cv::Point( row.minX, row.row ), cv::Point( row.maxX, row.row ), color, 1 );
		}
		for ( auto row : caseToSave->_rightRows ) {
			cv::Scalar color;
			if ( row.tag == 0 ) color = cv::Scalar( 0, 255, 0 );
			else if ( row.tag == 1 ) color = cv::Scalar( 0, 255, 255 );
			else if ( row.tag == -1 ) color = cv::Scalar( 0, 0, 255 );

			cv::line( img, cv::Point( row.minX, row.row ), cv::Point( row.maxX, row.row ), color, 1 );
		}
		cv::imshow( "Analyzer", img );
		cv::waitKey();
	}*/

	// calculate target: select the top most good row to calculate
	{
		auto cit_lrow = leftBlob->rows.crbegin();
		auto cit_rrow = rightBlob->rows.crbegin();

		for ( ; cit_lrow != leftBlob->rows.crend() && cit_rrow != rightBlob->rows.crend(); ++cit_lrow , ++cit_rrow ) {
			if ( cit_lrow->tag != 0 || cit_rrow->tag != 0 ) continue;

			int x1 = cit_lrow->maxX;
			int x2 = cit_rrow->minX;
			analyzeData->target = cv::Point( (x1 + x2) / 2, cit_lrow->row );
			break;
		}
	}

	return 0;
}

int sb::BothLaneSolidCase::trackAnalyze( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, AnalyzeData* analyzeData )
{
	Blob *leftBlob = nullptr, *rightBlob = nullptr;

	// find blob similar the most to this old left and right
	leftBlob = trackLeftBlob( caseRepository, collectData, calculateData );

	rightBlob = trackRightBlob( caseRepository, collectData, calculateData );

	if ( leftBlob == nullptr && rightBlob == nullptr ) {
		std::cerr << "BothLaneSolidCase: both lane disappear" << std::endl;
		return -1; // TODO: dashed lane
	}

	// store analyze data
	auto caseToSave = new BothLaneSolidCase( _params, _obstacleFinder );

	// check for left row lane width
	if ( leftBlob != nullptr ) {
		caseToSave->_leftLaneOrigin = leftBlob->origin;
		caseToSave->_leftLaneSize = static_cast<int>(leftBlob->size);
		caseToSave->_leftLaneHeight = leftBlob->box.height;
		caseToSave->_leftGoodSections.clear();
		caseToSave->_leftBadSections.clear(); {
			auto it_row = leftBlob->rows.begin();
			while ( it_row != leftBlob->rows.end() ) {
				auto pre_row = it_row;

				if ( (it_row->row >= calculateData->bgrImage.rows / 2 && (it_row->width < _params->MIN_LANE_WIDTH_1 || it_row->width > _params->MAX_LANE_WIDTH_1))
					|| (it_row->row < calculateData->bgrImage.rows / 2 && (it_row->width < _params->MIN_LANE_WIDTH_2 || it_row->width > _params->MAX_LANE_WIDTH_2)) ) {
					beginBadSection( it_row, leftBlob, calculateData );
					caseToSave->_leftBadSections.push_back( std::make_pair( static_cast<int>(std::distance( leftBlob->rows.begin(), pre_row )), static_cast<int>(std::distance( pre_row, it_row )) ) );
				}
				else {
					beginCorrectSection( it_row, leftBlob, calculateData );
					caseToSave->_leftGoodSections.push_back( std::make_pair( static_cast<int>(std::distance( leftBlob->rows.begin(), pre_row )), static_cast<int>(std::distance( pre_row, it_row )) ) );
				}
			}
		}
		caseToSave->_leftRows = leftBlob->rows;
	}

	// check for right row lane width
	if ( rightBlob != nullptr ) {
		caseToSave->_rightLaneOrigin = rightBlob->origin;
		caseToSave->_rightLaneSize = static_cast<int>(rightBlob->size);
		caseToSave->_rightLaneHeight = rightBlob->box.height;
		caseToSave->_rightGoodSections.clear();
		caseToSave->_rightGoodSections.clear();
		caseToSave->_rightBadSections.clear(); {
			auto it_row = rightBlob->rows.begin();
			while ( it_row != rightBlob->rows.end() ) {
				auto pre_row = it_row;

				if ( (it_row->row >= calculateData->bgrImage.rows / 2 && (it_row->width < _params->MIN_LANE_WIDTH_1 || it_row->width > _params->MAX_LANE_WIDTH_1))
					|| (it_row->row < calculateData->bgrImage.rows / 2 && (it_row->width < _params->MIN_LANE_WIDTH_2 || it_row->width > _params->MAX_LANE_WIDTH_2)) ) {
					beginBadSection( it_row, rightBlob, calculateData );
					caseToSave->_rightBadSections.push_back( std::make_pair( static_cast<int>(std::distance( rightBlob->rows.begin(), pre_row )), static_cast<int>(std::distance( pre_row, it_row )) ) );
				}
				else {
					beginCorrectSection( it_row, rightBlob, calculateData );
					caseToSave->_rightGoodSections.push_back( std::make_pair( static_cast<int>(std::distance( rightBlob->rows.begin(), pre_row )), static_cast<int>(std::distance( pre_row, it_row )) ) );
				}
			}
		}
		caseToSave->_rightRows = rightBlob->rows;
	}

	// redirect RIGHT_LANE_SOLID
	if ( leftBlob == nullptr ) {
		auto rightLaneSolidCase = new RightLaneSolidCase( _params, _obstacleFinder );
		int res = rightLaneSolidCase->onRedirect( caseRepository, collectData, calculateData, analyzeData, caseToSave );
		delete rightLaneSolidCase;
		delete caseToSave;
		return res;
	}

	// redirect LEFT_LANE_SOLID
	if ( rightBlob == nullptr ) {
		auto leftLaneSolidCase = new LeftLaneSolidCase( _params, _obstacleFinder );
		int res = leftLaneSolidCase->onRedirect( caseRepository, collectData, calculateData, analyzeData, caseToSave );
		delete leftLaneSolidCase;
		delete caseToSave;
		return res;
	}

	// push to repository
	caseRepository->push( caseToSave );

	// debug
	/*{
		cv::Mat img = calculateData->bgrImage.clone();
		for ( auto row : caseToSave->_leftRows ) {
			cv::Scalar color;
			if ( row.tag == 0 ) color = cv::Scalar( 0, 255, 0 );
			else if ( row.tag == 1 ) color = cv::Scalar( 0, 255, 255 );
			else if ( row.tag == -1 ) color = cv::Scalar( 0, 0, 255 );

			cv::line( img, cv::Point( row.minX, row.row ), cv::Point( row.maxX, row.row ), color, 1 );
		}
		for ( auto row : caseToSave->_rightRows ) {
			cv::Scalar color;
			if ( row.tag == 0 ) color = cv::Scalar( 0, 255, 0 );
			else if ( row.tag == 1 ) color = cv::Scalar( 0, 255, 255 );
			else if ( row.tag == -1 ) color = cv::Scalar( 0, 0, 255 );

			cv::line( img, cv::Point( row.minX, row.row ), cv::Point( row.maxX, row.row ), color, 1 );
		}
		cv::imshow( "Analyzer", img );
		cv::waitKey();
	}*/

	// check good section percentage
	float leftGoodRatio = 0;
	float rightGoodRatio = 0; {
		int sum;

		sum = 0;
		for ( auto sectionInfo : caseToSave->_leftGoodSections ) sum += sectionInfo.second;
		leftGoodRatio = 1.0f * sum / calculateData->bgrImage.rows;

		sum = 0;
		for ( auto sectionInfo : caseToSave->_rightGoodSections ) sum += sectionInfo.second;
		rightGoodRatio = 1.0f * sum / calculateData->bgrImage.rows;
	}

	if ( leftGoodRatio < 0.3f && rightGoodRatio < 0.3f ) {
		std::cerr << "BothLaneSolidCase: bad tracked lanes" << std::endl;
		return -1; // TODO: dashed lane
	}

	// calculate target base on left lane
	if ( rightGoodRatio < 0.3f ) {
		auto cit_lrow = caseToSave->_leftRows.crbegin();

		for ( ; cit_lrow != caseToSave->_leftRows.crend(); ++cit_lrow ) {
			if ( cit_lrow->tag != 0 ) continue;

			auto roadWidth = caseRepository->findRoadWidth( cit_lrow->row );
			if ( roadWidth < 0 ) continue;

			int xl = cit_lrow->maxX;
			analyzeData->target = cv::Point( xl + roadWidth / 5, cit_lrow->row );
			break;
		}
	}
	// calculate target base on right lane
	else if ( leftGoodRatio < 0.3f ) {
		auto cit_rrow = caseToSave->_rightRows.crbegin();

		for ( ; cit_rrow != caseToSave->_rightRows.crend(); ++cit_rrow ) {
			if ( cit_rrow->tag != 0 ) continue;

			auto roadWidth = caseRepository->findRoadWidth( cit_rrow->row );
			if ( roadWidth < 0 ) continue;

			int xr = cit_rrow->minX;
			analyzeData->target = cv::Point( xr - roadWidth / 4, cit_rrow->row );
			break;
		}
	}
	// calculate target with both lanes
	else {
		auto cit_lrow = caseToSave->_leftRows.crbegin();
		auto cit_rrow = caseToSave->_rightRows.crbegin();

		for ( ; cit_lrow != caseToSave->_leftRows.crend() && cit_rrow != caseToSave->_rightRows.crend(); ++cit_lrow , ++cit_rrow ) {
			if ( cit_lrow->tag != 0 || cit_rrow->tag != 0 ) continue;

			int x1 = cit_lrow->maxX;
			int x2 = cit_rrow->minX;
			analyzeData->target = cv::Point( (x1 + x2) / 2, cit_lrow->row );
			break;
		}
	}

	return 0;
}

int sb::BothLaneSolidCase::onRedirect( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, AnalyzeData* analyzeData, ICase* sender )
{
	switch ( sender->getType() ) {
	case CaseType::LEFT_LANE_SOLID_CASE: {
		LeftLaneSolidCase* leftLaneSolidCase = static_cast<LeftLaneSolidCase*>(sender);

		// store analyze data
		auto caseToSave = new BothLaneSolidCase( _params, _obstacleFinder );
		caseToSave->_leftLaneOrigin = leftLaneSolidCase->getLeftLaneOrigin();
		caseToSave->_leftLaneSize = leftLaneSolidCase->getLeftLaneSize();
		caseToSave->_leftLaneHeight = leftLaneSolidCase->getLeftLaneHeight();
		caseToSave->_leftBadSections = leftLaneSolidCase->getLeftBadSections();
		caseToSave->_leftGoodSections = leftLaneSolidCase->getLeftGoodSections();
		caseToSave->_leftRows = leftLaneSolidCase->getLeftRows();

		// find left lane
		Blob* rightBlob = nullptr;
		rightBlob = findRightBlob( caseRepository, collectData, calculateData, leftLaneSolidCase );

		// check for left row lane width
		if ( rightBlob != nullptr ) {
			caseToSave->_rightLaneOrigin = rightBlob->origin;
			caseToSave->_rightLaneSize = static_cast<int>(rightBlob->size);
			caseToSave->_rightLaneHeight = rightBlob->box.height;
			caseToSave->_rightGoodSections.clear();
			caseToSave->_rightBadSections.clear(); {
				auto it_row = rightBlob->rows.begin();
				while ( it_row != rightBlob->rows.end() ) {
					auto pre_row = it_row;

					if ( (it_row->row >= calculateData->bgrImage.rows / 2 && (it_row->width < _params->MIN_LANE_WIDTH_1 || it_row->width > _params->MAX_LANE_WIDTH_1))
						|| (it_row->row < calculateData->bgrImage.rows / 2 && (it_row->width < _params->MIN_LANE_WIDTH_2 || it_row->width > _params->MAX_LANE_WIDTH_2)) ) {
						beginBadSection( it_row, rightBlob, calculateData );
						caseToSave->_rightBadSections.push_back( std::make_pair( static_cast<int>(std::distance( rightBlob->rows.begin(), pre_row )), static_cast<int>(std::distance( pre_row, it_row )) ) );
					}
					else {
						beginCorrectSection( it_row, rightBlob, calculateData );
						caseToSave->_rightGoodSections.push_back( std::make_pair( static_cast<int>(std::distance( rightBlob->rows.begin(), pre_row )), static_cast<int>(std::distance( pre_row, it_row )) ) );
					}
				}
			}
			caseToSave->_rightRows = rightBlob->rows;
		}

		// redirect LEFT_LANE_SOLID
		if ( rightBlob == nullptr ) {
			auto newLeftLaneSolidCase = new LeftLaneSolidCase( _params, _obstacleFinder );
			int res = newLeftLaneSolidCase->onRedirect( caseRepository, collectData, calculateData, analyzeData, caseToSave );
			delete newLeftLaneSolidCase;
			delete caseToSave;
			return res;
		}

		// push to repository
		caseRepository->push( caseToSave );

		// check good section percentage
		float leftGoodRatio = 0;
		float rightGoodRatio = 0; {
			int sum;

			sum = 0;
			for ( auto sectionInfo : caseToSave->_leftGoodSections ) sum += sectionInfo.second;
			leftGoodRatio = 1.0f * sum / calculateData->bgrImage.rows;

			sum = 0;
			for ( auto sectionInfo : caseToSave->_rightGoodSections ) sum += sectionInfo.second;
			rightGoodRatio = 1.0f * sum / calculateData->bgrImage.rows;
		}

		if ( leftGoodRatio < 0.3f && rightGoodRatio < 0.3f ) {
			std::cerr << "BothLaneSolidCase: bad lanes" << std::endl;
			return -1; // TODO: dashed lane
		}

		// calculate target base on left lane
		if ( rightGoodRatio < 0.3f ) {
			auto cit_lrow = caseToSave->_leftRows.crbegin();

			for ( ; cit_lrow != caseToSave->_leftRows.crend(); ++cit_lrow ) {
				if ( cit_lrow->tag != 0 ) continue;

				auto roadWidth = caseRepository->findRoadWidth( cit_lrow->row );
				if ( roadWidth < 0 ) continue;

				int xl = cit_lrow->maxX;
				analyzeData->target = cv::Point( xl + roadWidth / 4, cit_lrow->row );
				break;
			}
		}
		// calculate target base on left lane
		else if ( leftGoodRatio < 0.3f ) {
			auto cit_rrow = caseToSave->_rightRows.crbegin();

			for ( ; cit_rrow != caseToSave->_rightRows.crend(); ++cit_rrow ) {
				if ( cit_rrow->tag != 0 ) continue;

				auto roadWidth = caseRepository->findRoadWidth( cit_rrow->row );
				if ( roadWidth < 0 ) continue;

				int xr = cit_rrow->minX;
				analyzeData->target = cv::Point( xr - roadWidth / 4, cit_rrow->row );
				break;
			}
		}
		// calculate target with both lanes
		else {
			auto cit_lrow = caseToSave->_leftRows.crbegin();
			auto cit_rrow = caseToSave->_rightRows.crbegin();

			for ( ; cit_lrow != caseToSave->_leftRows.crend() && cit_rrow != caseToSave->_rightRows.crend(); ++cit_lrow , ++cit_rrow ) {
				if ( cit_lrow->tag != 0 || cit_rrow->tag != 0 ) continue;

				int x1 = cit_lrow->maxX;
				int x2 = cit_rrow->minX;
				analyzeData->target = cv::Point( (x1 + x2) / 2, cit_lrow->row );
				break;
			}
		}
	}
		break;

	case CaseType::RIGHT_LANE_SOLID_CASE: {
		RightLaneSolidCase* rightLaneSolidCase = static_cast<RightLaneSolidCase*>(sender);

		// store analyze data
		auto caseToSave = new BothLaneSolidCase( _params, _obstacleFinder );
		caseToSave->_rightLaneOrigin = rightLaneSolidCase->getRightLaneOrigin();
		caseToSave->_rightLaneSize = rightLaneSolidCase->getRightLaneSize();
		caseToSave->_rightLaneHeight = rightLaneSolidCase->getRightLaneHeight();
		caseToSave->_rightBadSections = rightLaneSolidCase->getRightBadSections();
		caseToSave->_rightGoodSections = rightLaneSolidCase->getRightGoodSections();
		caseToSave->_rightRows = rightLaneSolidCase->getRightRows();

		// find left lane
		Blob* leftBlob = nullptr;
		leftBlob = findLeftBlob( caseRepository, collectData, calculateData, rightLaneSolidCase );

		// check for left row lane width
		if ( leftBlob != nullptr ) {
			caseToSave->_leftLaneOrigin = leftBlob->origin;
			caseToSave->_leftLaneSize = static_cast<int>(leftBlob->size);
			caseToSave->_leftLaneHeight = leftBlob->box.height;
			caseToSave->_leftGoodSections.clear();
			caseToSave->_leftBadSections.clear(); {
				auto it_row = leftBlob->rows.begin();
				while ( it_row != leftBlob->rows.end() ) {
					auto pre_row = it_row;

					if ( (it_row->row >= calculateData->bgrImage.rows / 2 && (it_row->width < _params->MIN_LANE_WIDTH_1 || it_row->width > _params->MAX_LANE_WIDTH_1))
						|| (it_row->row < calculateData->bgrImage.rows / 2 && (it_row->width < _params->MIN_LANE_WIDTH_2 || it_row->width > _params->MAX_LANE_WIDTH_2)) ) {
						beginBadSection( it_row, leftBlob, calculateData );
						caseToSave->_leftBadSections.push_back( std::make_pair( static_cast<int>(std::distance( leftBlob->rows.begin(), pre_row )), static_cast<int>(std::distance( pre_row, it_row )) ) );
					}
					else {
						beginCorrectSection( it_row, leftBlob, calculateData );
						caseToSave->_leftGoodSections.push_back( std::make_pair( static_cast<int>(std::distance( leftBlob->rows.begin(), pre_row )), static_cast<int>(std::distance( pre_row, it_row )) ) );
					}
				}
			}
			caseToSave->_leftRows = leftBlob->rows;
		}

		// redirect RIGHT_LANE_SOLID
		if ( leftBlob == nullptr ) {
			auto newRightLaneSolidCase = new RightLaneSolidCase( _params, _obstacleFinder );
			int res = newRightLaneSolidCase->onRedirect( caseRepository, collectData, calculateData, analyzeData, caseToSave );
			delete newRightLaneSolidCase;
			delete caseToSave;
			return res;
		}

		// push to repository
		caseRepository->push( caseToSave );

		// check good section percentage
		float leftGoodRatio = 0;
		float rightGoodRatio = 0; {
			int sum;

			sum = 0;
			for ( auto sectionInfo : caseToSave->_leftGoodSections ) sum += sectionInfo.second;
			leftGoodRatio = 1.0f * sum / calculateData->bgrImage.rows;

			sum = 0;
			for ( auto sectionInfo : caseToSave->_rightGoodSections ) sum += sectionInfo.second;
			rightGoodRatio = 1.0f * sum / calculateData->bgrImage.rows;
		}

		if ( leftGoodRatio < 0.3f && rightGoodRatio < 0.3f ) {
			std::cerr << "BothLaneSolidCase: bad lanes" << std::endl;
			return -1; // TODO: dashed lane
		}

		// calculate target base on left lane
		if ( rightGoodRatio < 0.3f ) {
			auto cit_lrow = caseToSave->_leftRows.crbegin();

			for ( ; cit_lrow != caseToSave->_leftRows.crend(); ++cit_lrow ) {
				if ( cit_lrow->tag != 0 ) continue;

				auto roadWidth = caseRepository->findRoadWidth( cit_lrow->row );
				if ( roadWidth < 0 ) continue;

				int xl = cit_lrow->maxX;
				analyzeData->target = cv::Point( xl + roadWidth / 5, cit_lrow->row );
				break;
			}
		}
		// calculate target base on right lane
		else if ( leftGoodRatio < 0.3f ) {
			auto cit_rrow = caseToSave->_rightRows.crbegin();

			for ( ; cit_rrow != caseToSave->_rightRows.crend(); ++cit_rrow ) {
				if ( cit_rrow->tag != 0 ) continue;

				auto roadWidth = caseRepository->findRoadWidth( cit_rrow->row );
				if ( roadWidth < 0 ) continue;

				int xr = cit_rrow->minX;
				analyzeData->target = cv::Point( xr - roadWidth / 5, cit_rrow->row );
				break;
			}
		}
		// calculate target with both lanes
		else {
			auto cit_lrow = caseToSave->_leftRows.crbegin();
			auto cit_rrow = caseToSave->_rightRows.crbegin();

			for ( ; cit_lrow != caseToSave->_leftRows.crend() && cit_rrow != caseToSave->_rightRows.crend(); ++cit_lrow , ++cit_rrow ) {
				if ( cit_lrow->tag != 0 || cit_rrow->tag != 0 ) continue;

				int x1 = cit_lrow->maxX;
				int x2 = cit_rrow->minX;
				analyzeData->target = cv::Point( (x1 + x2) / 2, cit_lrow->row );
				break;
			}
		}

	}
		break;
	}

	return 0;
}

int sb::BothLaneSolidCase::findTwoLargestBlobs( CollectData* collectData, CalculateData* calculateData, Blob* largestBlobs[2] ) const
{
	if ( calculateData->blobs.empty() ) return -1;

	for ( auto cit_blob = calculateData->blobs.cbegin(); cit_blob != calculateData->blobs.cend(); ++cit_blob ) {
		sb::Blob* blob = *cit_blob;

		if ( largestBlobs[0] == nullptr || blob->size > largestBlobs[0]->size ) {
			largestBlobs[1] = largestBlobs[0];
			largestBlobs[0] = blob;
		}
		else if ( largestBlobs[1] == nullptr || blob->size > largestBlobs[1]->size ) {
			largestBlobs[1] = blob;
		}
	}

	return 0;
}

void sb::BothLaneSolidCase::beginBadSection( std::vector<BlobRow>::iterator& it_row, Blob* blob, CalculateData* calculateData ) const
{
	int correctRow = 0;
	int correctWidth = -1;

	while ( it_row != blob->rows.cend() ) {

		if ( (it_row->row >= calculateData->bgrImage.rows / 2 && it_row->width >= _params->MIN_LANE_WIDTH_1 && it_row->width <= _params->MAX_LANE_WIDTH_1)
			|| (it_row->row < calculateData->bgrImage.rows / 2 && it_row->width >= _params->MIN_LANE_WIDTH_2 && it_row->width <= _params->MAX_LANE_WIDTH_2) ) {
			if ( correctRow == 0 ) {
				correctWidth = it_row->width;
				correctRow++;
			}
			else if ( correctRow > _params->SECTION_HOPS_TO_LIVE ) {
				break;
			}
			else {
				if ( abs( it_row->width - correctWidth ) < _params->MAX_ROW_WIDTH_DIFF ) {
					correctRow++;
				}
				else {
					correctRow = 0;
				}
			}
		}
		else {
			correctRow = 0;
		}
		it_row->tag = -1;
		++it_row;
	}
}

void sb::BothLaneSolidCase::beginCorrectSection( std::vector<BlobRow>::iterator& it_row, Blob* blob, CalculateData* calculateData ) const
{
	auto rowToCompare = it_row;
	int failTimes = 0;

	while ( it_row != blob->rows.cend() ) {

		if ( abs( it_row->width - rowToCompare->width ) < _params->MAX_ROW_WIDTH_DIFF ) {
			it_row->tag = 0;
			failTimes = 0;
		}
		else if ( it_row->width < _params->MIN_LANE_WIDTH_1 ) {
			it_row->tag = 1;
			failTimes = 0;
		}
		else {
			if ( failTimes <= _params->SECTION_HOPS_TO_LIVE ) {
				failTimes++;
			}
			else {
				break;
			}
			it_row->tag = 1;
		}

		if ( std::distance( rowToCompare, it_row ) >= 10 ) {
			rowToCompare = it_row - 10;
			while ( rowToCompare->tag != 0 && rowToCompare != it_row ) ++rowToCompare;

			if ( rowToCompare->tag != 0 ) break;
		}
		++it_row;
	}
}

sb::Blob* sb::BothLaneSolidCase::trackLeftBlob( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData )
{
	std::vector<std::pair<Blob*, float>> possibleBlobs;

	for ( auto cit_blob = calculateData->blobs.cbegin(); cit_blob != calculateData->blobs.cend(); ++cit_blob ) {
		Blob* blob = *cit_blob;

		// check blob size
		if ( blob->size < _params->MIN_LANE_BLOB_SIZE ) continue;

		// check blob height
		if ( blob->box.height < _params->MIN_LANE_BLOB_HEIGHT ) continue;

		// compare position
		int posDiff = abs( blob->origin.x - _leftLaneOrigin.x );
		if ( posDiff > _params->MAX_LANE_POSITION_DIFF ) continue;

		// compare size
		int sizeDiff = abs( static_cast<int>(blob->size) - _leftLaneSize );
		if ( sizeDiff > _params->MAX_LANE_SIZE_DIFF ) continue;

		// compare height
		int heightDiff = abs( blob->box.height - _leftLaneHeight );
		if ( heightDiff > _params->MAX_LANE_HEIGHT_DIFF ) continue;

		float rating = 0.4f * posDiff + 0.3f * sizeDiff + 0.3f * heightDiff;
		possibleBlobs.push_back( std::make_pair( blob, rating ) );
		// TODO: compare shape (row width, angle)

		// TODO: save and compare blob knot (row width, angle)

	}

	if ( possibleBlobs.empty() ) return nullptr;

	std::pair<Blob*, float> resPair = possibleBlobs.front();
	for ( auto cit_pair = possibleBlobs.cbegin(); cit_pair != possibleBlobs.cend(); ++cit_pair ) {
		if ( cit_pair->second > resPair.second ) {
			resPair = *cit_pair;
		}
	}

	return resPair.first;
}

sb::Blob* sb::BothLaneSolidCase::trackRightBlob( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData )
{
	std::vector<std::pair<Blob*, float>> possibleBlobs;

	for ( auto cit_blob = calculateData->blobs.cbegin(); cit_blob != calculateData->blobs.cend(); ++cit_blob ) {
		Blob* blob = *cit_blob;

		// check blob size
		if ( blob->size < _params->MIN_LANE_BLOB_SIZE ) continue;

		// check blob height
		if ( blob->box.height < _params->MIN_LANE_BLOB_HEIGHT ) continue;

		// compare position
		int posDiff = abs( blob->origin.x - _rightLaneOrigin.x );
		if ( posDiff > _params->MAX_LANE_POSITION_DIFF ) continue;

		// compare size
		int sizeDiff = abs( static_cast<int>(blob->size) - _rightLaneSize );
		if ( sizeDiff > _params->MAX_LANE_SIZE_DIFF ) continue;

		// compare height
		int heightDiff = abs( blob->box.height - _rightLaneHeight );
		if ( heightDiff > _params->MAX_LANE_HEIGHT_DIFF ) continue;

		float rating = 0.4f * posDiff + 0.3f * sizeDiff + 0.3f * heightDiff;
		possibleBlobs.push_back( std::make_pair( blob, rating ) );
		// TODO: compare shape (row width, angle)

		// TODO: save and compare blob knot (row width, angle)

	}

	if ( possibleBlobs.empty() ) return nullptr;

	std::pair<Blob*, float> resPair = possibleBlobs.front();
	for ( auto cit_pair = possibleBlobs.cbegin(); cit_pair != possibleBlobs.cend(); ++cit_pair ) {
		if ( cit_pair->second > resPair.second ) {
			resPair = *cit_pair;
		}
	}

	return resPair.first;
}

sb::Blob* sb::BothLaneSolidCase::findLeftBlob( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, RightLaneSolidCase* rightLaneSolidCase )
{
	for ( auto cit_blob = calculateData->blobs.cbegin(); cit_blob != calculateData->blobs.cend(); ++cit_blob ) {
		auto blob = *cit_blob;

		// check blob size
		if ( blob->size < _params->MIN_LANE_BLOB_SIZE ) continue;

		// check blob height
		if ( blob->box.height < _params->MIN_LANE_BLOB_HEIGHT ) continue;

		if ( blob->origin.x > rightLaneSolidCase->getRightLaneOrigin().x - 50 ) continue;

		// TODO: check shape (rows pos and width)
		return blob;
	}

	return nullptr;
}

sb::Blob* sb::BothLaneSolidCase::findRightBlob( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, LeftLaneSolidCase* leftLaneSolidCase )
{
	for ( auto cit_blob = calculateData->blobs.cbegin(); cit_blob != calculateData->blobs.cend(); ++cit_blob ) {
		auto blob = *cit_blob;

		// check blob size
		if ( blob->size < _params->MIN_LANE_BLOB_SIZE ) continue;

		// check blob height
		if ( blob->box.height < _params->MIN_LANE_BLOB_HEIGHT ) continue;

		if ( blob->origin.x < leftLaneSolidCase->getLeftLaneOrigin().x + 50 ) continue;

		// TODO: check shape (rows pos and width)
		return blob;
	}

	return nullptr;
}
