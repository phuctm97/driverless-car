#ifndef __SB_REPOSITORY_H__
#define __SB_REPOSITORY_H__

#include "../calculator/Blob.h"

namespace sb
{
struct Repository
{
	sb::Blob* leftBlob = nullptr;

	sb::Blob* rightBlob = nullptr;

	std::vector<int> roadWidths;

	std::vector<int> possibleNextStates;
};

void release( sb::Repository* repo );
}

#endif //!__SB_REPOSITORY_H__
