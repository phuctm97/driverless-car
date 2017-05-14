#ifndef __SB_CALCULATOR_BLOBS_BASED_H__
#define __SB_CALCULATOR_BLOBS_BASED_H__

#include "../ICalculator.h"
#include "../ICropTool.h"
#include "../IFlipTool.h"
#include "../IBinarizeTool.h"
#include "../IBlobTool.h"

namespace sb
{
class CalculatorBlobsBased : public ICalculator
{
private:
	ICropTool* _cropTool;
	IFlipTool* _flipTool;
	IBinarizeTool* _binarizeTool;
	IBlobTool* _blobTool;

public:
	CalculatorBlobsBased( ICropTool* cropTool, IFlipTool* flipTool, IBinarizeTool* binarizeTool, IBlobTool* blobTool )
		: _cropTool( cropTool ),
		  _flipTool( flipTool ),
		  _binarizeTool( binarizeTool ),
		  _blobTool( blobTool ) {}

	int init() override;

	int calculate( CollectData* collectData, CalculateData* calculateData ) override;

	void release() override;
};
}

#endif //!__SB_CALCULATOR_BLOBS_BASED_H__
