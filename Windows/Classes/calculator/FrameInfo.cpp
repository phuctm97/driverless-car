#include "FrameInfo.h"

void sb::construct( sb::FrameInfo*& frameInfo )
{
	frameInfo = new FrameInfo;
}

void sb::destruct( sb::FrameInfo*& frameInfo )
{
	delete frameInfo;
	frameInfo = nullptr;
}

void sb::create( sb::FrameInfo* frameInfo, sb::Params* params ) {}
