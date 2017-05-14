#ifndef __SB_BOTH_LANE_SOLID_CASE_H__
#define __SB_BOTH_LANE_SOLID_CASE_H__

#include "ICase.h"
#include "../../Data/Params/AnalyzeParams.h"
#include "RightLaneSolidCase.h"
#include "LeftLaneSolidCase.h"
#include "../ObstacleFinder.h"

namespace sb
{
class BothLaneSolidCase : public ICase
{
private:
	AnalyzeParams* _params;
	ObstacleFinder* _obstacleFinder;

	// analyze data
	cv::Point _leftLaneOrigin, _rightLaneOrigin;
	int _leftLaneSize, _rightLaneSize;
	int _leftLaneHeight, _rightLaneHeight;
	std::vector<BlobRow> _leftRows, _rightRows;
	std::vector<std::pair<int, int>> _leftGoodSections, _leftBadSections, _rightGoodSections, _rightBadSections;

	int _obstaclePosition;


public:
	// ReSharper disable CppPossiblyUninitializedMember
	BothLaneSolidCase( AnalyzeParams* params, ObstacleFinder* obstacleFinder )
	// ReSharper restore CppPossiblyUninitializedMember
		: _params( params ), _obstacleFinder( obstacleFinder ) { }

	const cv::Point& getLeftLaneOrigin() const;

	const cv::Point& getRightLaneOrigin() const;

	const int& getLeftLaneSize() const;

	const int& getRightLaneSize() const;

	const int& getLeftLaneHeight() const;

	const int& getRightLaneHeight() const;

	const std::vector<BlobRow>& getLeftRows() const;

	const std::vector<BlobRow>& getRightRows() const;

	const std::vector<std::pair<int, int>>& getLeftGoodSections() const;

	const std::vector<std::pair<int, int>>& getLeftBadSections() const;

	const std::vector<std::pair<int, int>>& getRightGoodSections() const;

	const std::vector<std::pair<int, int>>& getRightBadSections() const;

	const int& getObstaclePosition() const;

	int getType() override;

	int analyze( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, AnalyzeData* analyzeData ) override;

	int onRedirect( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, AnalyzeData* analyzeData, ICase* sender );

private:
	int firstAnalyze( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, AnalyzeData* analyzeData );

	int trackAnalyze( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, AnalyzeData* analyzeData );

	int findTwoLargestBlobs( CollectData* collectData, CalculateData* calculateData, Blob* largestBlobs[2] ) const;

	void beginBadSection( std::vector<BlobRow>::iterator& it_row, Blob* blob, CalculateData* calculateData ) const;

	void beginCorrectSection( std::vector<BlobRow>::iterator& it_row, Blob* blob, CalculateData* calculateData ) const;

	Blob* trackLeftBlob( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData );

	Blob* trackRightBlob( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData );

	Blob* findLeftBlob( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, RightLaneSolidCase* rightLaneSolidCase );

	Blob* findRightBlob( CaseRepository* caseRepository, CollectData* collectData, CalculateData* calculateData, LeftLaneSolidCase* leftLaneSolidCase );
};
}

#endif //!__SB_BOTH_LANE_SOLID_CASE_H__
