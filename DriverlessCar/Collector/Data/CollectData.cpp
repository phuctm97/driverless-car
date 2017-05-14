#include "CollectData.h"

void sb::release( CollectData* collectData )
{
	collectData->colorImage.release();
}
