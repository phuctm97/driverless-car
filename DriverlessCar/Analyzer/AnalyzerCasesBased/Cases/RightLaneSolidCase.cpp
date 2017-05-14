#include "RightLaneSolidCase.h"
#include "../CaseRepository.h"
#include "BothLaneSolidCase.h"

sb::AnalyzeParams* const& sb::RightLaneSolidCase::getParams() const { return _params; }

const cv::Point& sb::RightLaneSolidCase::getRightLaneOrigin() const { return _rightLaneOrigin; }

const int& sb::RightLaneSolidCase::getRightLaneSize() const { return _rightLaneSize; }

const int& sb::RightLaneSolidCase::getRightLaneHeight() const { return _rightLaneHeight; }

const std::vector<sb::BlobRow>& sb::RightLaneSolidCase::getRightRows() const { return _rightRows; }

const std::vector<std::pair<int, int>>& sb::RightLaneSolidCase::getRightGoodSections() const { return _rightGoodSections; }

const std::vector<std::pair<int, int>>& sb::RightLaneSolidCase::getRightBadSections() const { return _rightBadSections; }

int sb::RightLaneSolidCase::getType()
{
	return CaseType::RIGHT_LANE_SOLID_CASE;
}

int sb::RightLaneSolidCase::analyze( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, AnalyzeData* analyzeData )
{
	return trackAnalyze( caseRepository, collectData, calculateData, analyzeData );
}

int sb::RightLaneSolidCase::trackAnalyze( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, AnalyzeData* analyzeData )
{
	Blob* rightBlob = nullptr;

	// find blob similar the most to this old left
	rightBlob = trackRightBlob( caseRepository, collectData, calculateData );

	// error
	if ( rightBlob == nullptr ) {
		std::cerr << "RightLaneSolidCase: right lane disappear" << std::endl;
		return -1; // TODO: dashed lane
	}

	// save size, height, shape: save to this, and push this to repository
	auto caseToSave = new RightLaneSolidCase( _params, _obstacleFinder );
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

	// check both lane case possible( relied on new blob position and road width in repository )
	bool bothLaneSolidPossible = true;
	for ( auto cit_row = caseToSave->_rightRows.cbegin(); cit_row != caseToSave->_rightRows.cend(); ++cit_row ) {
		int roadWidth = caseRepository->findRoadWidth( cit_row->row );
		if ( roadWidth < 0 ) continue;

		if ( cit_row->minX - roadWidth < -10 ) {
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
	float rightGoodRatio = 0; {
		int sum;

		sum = 0;
		for ( auto sectionInfo : caseToSave->_rightGoodSections ) sum += sectionInfo.second;
		rightGoodRatio = 1.0f * sum / calculateData->bgrImage.rows;
	}

	if ( rightGoodRatio < 0.3f ) {
		std::cerr << "RightLaneSolidCase: bad tracked lane" << std::endl;
		return -1; // TODO: dashed lane
	}

	// calculate target base on right lane
	auto cit_rrow = rightBlob->rows.crbegin();

	for ( ; cit_rrow != rightBlob->rows.crend(); ++cit_rrow ) {
		if ( cit_rrow->tag != 0 ) continue;

		auto roadWidth = caseRepository->findRoadWidth( cit_rrow->row );
		if ( roadWidth < 0 ) continue;

		int xr = cit_rrow->minX;
		analyzeData->target = cv::Point( xr - roadWidth / 4, cit_rrow->row );
		break;
	}

	return 0;
}

sb::Blob* sb::RightLaneSolidCase::trackRightBlob( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData )
{
	std::vector<std::pair<Blob*, float>> possibleBlobs;

	for( auto cit_blob = calculateData->blobs.cbegin(); cit_blob != calculateData->blobs.cend(); ++cit_blob ) {
		Blob* blob = *cit_blob;

		// check blob size
		if( blob->size < _params->MIN_LANE_BLOB_SIZE ) continue;

		// check blob height
		if( blob->box.height < _params->MIN_LANE_BLOB_HEIGHT ) continue;

		// compare position
		int posDiff = abs( blob->origin.x - _rightLaneOrigin.x );
		if( posDiff > _params->MAX_LANE_POSITION_DIFF ) continue;

		// compare size
		int sizeDiff = abs( static_cast<int>(blob->size) - _rightLaneSize );
		if( sizeDiff > _params->MAX_LANE_SIZE_DIFF ) continue;

		// compare height
		int heightDiff = abs( blob->box.height - _rightLaneHeight );
		if( heightDiff > _params->MAX_LANE_HEIGHT_DIFF ) continue;

		float rating = 0.4f * posDiff + 0.3f * sizeDiff + 0.3f * heightDiff;
		possibleBlobs.push_back( std::make_pair( blob, rating ) );
		// TODO: compare shape (row width, angle)

		// TODO: save and compare blob knot (row width, angle)

	}

	if( possibleBlobs.empty() ) return nullptr;

	std::pair<Blob*, float> resPair = possibleBlobs.front();
	for( auto cit_pair = possibleBlobs.cbegin(); cit_pair != possibleBlobs.cend(); ++cit_pair ) {
		if( cit_pair->second > resPair.second ) {
			resPair = *cit_pair;
		}
	}

	return resPair.first;
}

int sb::RightLaneSolidCase::onRedirect( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, AnalyzeData* analyzeData, ICase* sender )
{
	switch ( sender->getType() ) {
	case CaseType::BOTH_LANE_SOLID_CASE: {
		auto bothSolidLaneCase = static_cast<BothLaneSolidCase*>(sender);

		// check good section percentage
		float rightGoodRatio = 0; {
			int sum;
			sum = 0;
			for ( auto sectionInfo : bothSolidLaneCase->getRightGoodSections() ) sum += sectionInfo.second;
			rightGoodRatio = 1.0f * sum / calculateData->bgrImage.rows;
		}
		if ( rightGoodRatio < 0.3f ) {
			std::cerr << "RightLaneSolidCase: bad right lane" << std::endl;
			return -1; // TODO: dashed lane
		}

		// save size, height, shape: save to this, and push this to repository
		auto caseToSave = new RightLaneSolidCase( _params, _obstacleFinder );
		caseToSave->_rightLaneOrigin = bothSolidLaneCase->getRightLaneOrigin();
		caseToSave->_rightLaneSize = bothSolidLaneCase->getRightLaneSize();
		caseToSave->_rightLaneHeight = bothSolidLaneCase->getRightLaneHeight();
		caseToSave->_rightBadSections = bothSolidLaneCase->getRightBadSections();
		caseToSave->_rightGoodSections = bothSolidLaneCase->getRightGoodSections();
		caseToSave->_rightRows = bothSolidLaneCase->getRightRows();
		caseRepository->push( caseToSave );

		// use lane calculated in BOTH_LANE and width in repository calculate target
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
		break;
	default: return -1;
	}

	return 0;
}

void sb::RightLaneSolidCase::beginBadSection( std::vector<BlobRow>::iterator& it_row, Blob* blob, CalculateData* calculateData ) const
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

void sb::RightLaneSolidCase::beginCorrectSection( std::vector<BlobRow>::iterator& it_row, Blob* blob, CalculateData* calculateData ) const
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
