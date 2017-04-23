#include "Repository.h"

void sb::release( sb::Repository* repo )
{
	if ( repo->leftBlob != nullptr ) {
		sb::release( repo->leftBlob );
		delete repo->leftBlob;
		repo->leftBlob = nullptr;
	}

	if ( repo->rightBlob != nullptr ) {
		sb::release( repo->rightBlob );
		delete repo->rightBlob;
		repo->rightBlob = nullptr;
	}
}
