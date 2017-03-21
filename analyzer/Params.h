#ifndef __SB_PARAMS_H__
#define __SB_PARAMS_H__

#include "Include.h"

namespace sb
{
class Params
{
public:
	const static cv::String YAML_FIELD_NAME;

private:

	cv::Rect _areaToCrop;
	std::vector<float> _ratioToSplit;

	int _edgeDetectorKernelSize;
	double _edgeDetectorLowThresh;
	double _edgeDetectorHighThresh;
	double _edgeDetectorBinarizeThresh;
	double _edgeDetectorBinarizeMaxValue;

	double _houghLinesPRho;
	double _houghLinesPTheta;
	int _houghLinesPThreshold;
	double _houghLinesPMinLineLength;
	double _houghLinesPMaxLineGap;

public:
	explicit Params()
		:
		_areaToCrop( 0, 0, 0, 0 ),
		_ratioToSplit( { 0.15f,0.2f,0.2f,0.2f,0.25f } ),

		_edgeDetectorKernelSize( 3 ),
		_edgeDetectorLowThresh( 100 ),
		_edgeDetectorHighThresh( 200 ),
		_edgeDetectorBinarizeThresh( 200 ),
		_edgeDetectorBinarizeMaxValue( 255 ),

		_houghLinesPRho( 1 ),
		_houghLinesPTheta( CV_2PI / 360 ),
		_houghLinesPThreshold( 10 ),
		_houghLinesPMinLineLength( 2 ),
		_houghLinesPMaxLineGap( 7 ) {}

	explicit Params( const cv::String& yamlFileName );

	const cv::Rect& getAreaToCrop() const;

	void setAreaToCrop( const cv::Rect& areaToCrop );

	const std::vector<float>& getRatioToSplit() const;

	void setRatioToSplit( const std::vector<float>& sectionRatios );

	int getEdgeDetectorKernelSize() const;

	void setEdgeDetectorKernelSize( int edgeDetectorKernelSize );

	double getEdgeDetectorLowThresh() const;

	void setEdgeDetectorLowThresh( double edgeDetectorLowThresh );

	double getEdgeDetectorHighThresh() const;

	void setEdgeDetectorHighThresh( double edgeDetectorHighThresh );

	double getEdgeDetectorBinarizeThresh() const;

	void setEdgeDetectorBinarizeThresh( double edgeDetectorBinarizeThresh );

	double getEdgeDetectorBinarizeMaxValue() const;

	void setEdgeDetectorBinarizeMaxValue( double edgeDetectorBinarizeMaxValue );

	double getHoughLinesPRho() const;

	void setHoughLinesPRho( double houghLinesPRho );

	double getHoughLinesPTheta() const;

	void setHoughLinesPTheta( double houghLinesPTheta );

	int getHoughLinesPThreshold() const;

	void setHoughLinesPThreshold( int houghLinesPThreshold );

	double getHoughLinesPMinLineLength() const;

	void setHoughLinesPMinLineLength( double houghLinesPMinLineLength );

	double getHoughLinesPMaxLineGap() const;

	void setHoughLinesPMaxLineGap( double houghLinesPMaxLineGap );

	// yaml reader/writer functions
	void write( cv::FileStorage& fs ) const;

	void read( const cv::FileNode& node );
};

void write( cv::FileStorage& fs, const std::string&, const sb::Params& data );

void read( const cv::FileNode& node, sb::Params& data, const sb::Params& defaultData );
}

#endif //!__SB_PARAMS_H__
