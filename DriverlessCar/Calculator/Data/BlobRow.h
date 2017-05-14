#ifndef __SB_BLOB_ROW_H__
#define __SB_BLOB_ROW_H__

namespace sb
{
struct BlobRow
{
	int minX = -1;

	int maxX = -1;

	int width = -1;

	int row = -1;

	int tag = 111;
};

}

#endif //!__SB_BLOB_ROW_H__