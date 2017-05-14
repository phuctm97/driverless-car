#ifndef __SB_BLOB_TOOL_H__
#define __SB_BLOB_TOOL_H__

#define MIN_ACCEPTABLE_BLOB_OBJECTS_COUNT 20

#include "IBlobTool.h"

namespace sb
{
class BlobTool: public IBlobTool
{
private:
	std::vector<cv::Rect> _splitBoxes;

public:
	BlobTool( const std::vector<double>& splitRatio, const cv::Size& imageSize );

	int findBlobs( const cv::Mat& binImage, std::vector<sb::Blob*>& blobs ) override;
};
}

#endif //!__SB_BLOB_TOOL_H__
