#include "LeftLaneSolidCase.h"
#include "../CaseRepository.h"
#include "BothLaneSolidCase.h"

sb::AnalyzeParams* const& sb::LeftLaneSolidCase::getParams() const { return _params; }

const cv::Point& sb::LeftLaneSolidCase::getLeftLaneOrigin() const { return _leftLaneOrigin; }

const int& sb::LeftLaneSolidCase::getLeftLaneSize() const { return _leftLaneSize; }

const int& sb::LeftLaneSolidCase::getLeftLaneHeight() const { return _leftLaneHeight; }

const std::vector<sb::BlobRow>& sb::LeftLaneSolidCase::getLeftRows() const { return _leftRows; }

const std::vector<std::pair<int, int>>& sb::LeftLaneSolidCase::getLeftGoodSections() const { return _leftGoodSections; }

const std::vector<std::pair<int, int>>& sb::LeftLaneSolidCase::getLeftBadSections() const { return _leftBadSections; }

int sb::LeftLaneSolidCase::getType()
{
	return CaseType::LEFT_LANE_SOLID_CASE;
}

int sb::LeftLaneSolidCase::analyze( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, AnalyzeData* analyzeData )
{
	return trackAnalyze( caseRepository, collectData, calculateData, analyzeData );
}

int sb::LeftLaneSolidCase::trackAnalyze( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, AnalyzeData* analyzeData )
{
	Blob* leftBlob = nullptr;

	// find blob similar the most to this old left
	leftBlob = trackLeftBlob( caseRepository, collectData, calculateData );

	// error
	if ( leftBlob == nullptr ) {
		std::cerr << "LeftLaneSolidCase: left lane disappear" << std::endl;
		return -1; // TODO: dashed lane
	}

	// save size, height, shape: save to this, and push this to repository
	auto caseToSave = new LeftLaneSolidCase( _params, _obstacleFinder );
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

	// check both lane case possible( relied on new blob position and road width in repository )
	bool bothLaneSolidPossible = true;
	for ( auto cit_row = caseToSave->_leftRows.cbegin(); cit_row != caseToSave->_leftRows.cend(); ++cit_row ) {
		int roadWidth = caseRepository->findRoadWidth( cit_row->row );
		if ( roadWidth < 0 ) continue;

		if ( cit_row->maxX + roadWidth > calculateData->bgrImage.cols + 10 ) {
			bothLaneSolidPossible = false;
			break;
		}
	}

	// redirect BOTH_LANE_SOLID
	if ( bothLaneSolidPossible ) {
		auto bothLaneSolidCase = new BothLaneSolidCase( _params, _obstacleFinder );
		int res = bothLaneSolidCase->onRedirect( caseRepository, collectData, calculateData, analyzeData, caseToSave );
		delete bothLaneSolidCase;
		delete caseToSave;
		return res;
	}

	// push to repository
	caseRepository->push( caseToSave );

	// check good section percentage
	float leftGoodRatio = 0; {
		int sum;

		sum = 0;
		for ( auto sectionInfo : caseToSave->_leftGoodSections ) sum += sectionInfo.second;
		leftGoodRatio = 1.0f * sum / calculateData->bgrImage.rows;
	}

	if ( leftGoodRatio < 0.3f ) {
		std::cerr << "LeftLaneSolidCase: bad tracked lane" << std::endl;
		return -1; // TODO: dashed lane
	}

	// calculate target base on left lane
	auto cit_lrow = leftBlob->rows.crbegin();

	for ( ; cit_lrow != leftBlob->rows.crend(); ++cit_lrow ) {
		if ( cit_lrow->tag != 0 ) continue;

		auto roadWidth = caseRepository->findRoadWidth( cit_lrow->row );
		if ( roadWidth < 0 ) continue;

		int xl = cit_lrow->maxX;
		analyzeData->target = cv::Point( xl + roadWidth / 4, cit_lrow->row );
		break;
	}

	return 0;
}

sb::Blob* sb::LeftLaneSolidCase::trackLeftBlob( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData )
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
		if( cit_pair->second > resPair.second ) {
			resPair = *cit_pair;
		}
	}

	return resPair.first;
}

int sb::LeftLaneSolidCase::onRedirect( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, AnalyzeData* analyzeData, ICase* sender )
{
	switch ( sender->getType() ) {
	case CaseType::BOTH_LANE_SOLID_CASE: {
		auto bothSolidLaneCase = static_cast<BothLaneSolidCase*>(sender);

		// check good section percentage
		float leftGoodRatio = 0; {
			int sum;
			sum = 0;
			for ( auto sectionInfo : bothSolidLaneCase->getLeftGoodSections() ) sum += sectionInfo.second;
			leftGoodRatio = 1.0f * sum / calculateData->bgrImage.rows;
		}
		if ( leftGoodRatio < 0.3f ) {
			std::cerr << "LeftLaneSolidCase: bad left lane" << std::endl;
			return -1; // TODO: dashed lane
		}

		// save size, height, shape: save to this, and push this to repository
		auto caseToSave = new LeftLaneSolidCase( _params, _obstacleFinder );
		caseToSave->_leftLaneOrigin = bothSolidLaneCase->getLeftLaneOrigin();
		caseToSave->_leftLaneSize = bothSolidLaneCase->getLeftLaneSize();
		caseToSave->_leftLaneHeight = bothSolidLaneCase->getLeftLaneHeight();
		caseToSave->_leftBadSections = bothSolidLaneCase->getLeftBadSections();
		caseToSave->_leftGoodSections = bothSolidLaneCase->getLeftGoodSections();
		caseToSave->_leftRows = bothSolidLaneCase->getLeftRows();
		caseRepository->push( caseToSave );

		// use lane calculated in BOTH_LANE and width in repository calculate target
		auto cit_lrow = caseToSave->_leftRows.crbegin();

		for ( ; cit_lrow != caseToSave->_leftRows.crend(); ++cit_lrow ) {
			if ( cit_lrow->tag != 0 ) continue;

			auto roadWidth = caseRepository->findRoadWidth( cit_lrow->row );
			if ( roadWidth < 0 ) continue;

			int xl = cit_lrow->maxX;
			analyzeData->target = cv::Point( xl + roadWidth / 4, cit_lrow->row );
		}
	}
		break;
	default: return -1;
	}

	return 0;
}

void sb::LeftLaneSolidCase::beginBadSection( std::vector<BlobRow>::iterator& it_row, Blob* blob, CalculateData* calculateData ) const
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

void sb::LeftLaneSolidCase::beginCorrectSection( std::vector<BlobRow>::iterator& it_row, Blob* blob, CalculateData* calculateData ) const
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
