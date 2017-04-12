#include "Blob.h"

void sb::release( sb::Blob* blob )
{
	blob->pixels.clear();
}
