#include "Params.h"

const cv::String sb::Params::YAML_FIELD_NAME = "Params";

sb::Params::Params( const cv::String& yamlFileName )
{
	cv::FileStorage fs( yamlFileName, cv::FileStorage::READ );

	fs[YAML_FIELD_NAME] >> *this;

	fs.release();
}

const cv::Rect& sb::Params::getAreaToCrop() const { return _areaToCrop; }

void sb::Params::setAreaToCrop( const cv::Rect& areaToCrop ) { _areaToCrop = areaToCrop; }

const std::vector<float>& sb::Params::getRatioToSplit() const { return _ratioToSplit; }

void sb::Params::setRatioToSplit( const std::vector<float>& sectionRatios ) { _ratioToSplit = sectionRatios; }

int sb::Params::getEdgeDetectorKernelSize() const { return _edgeDetectorKernelSize; }

void sb::Params::setEdgeDetectorKernelSize( int edgeDetectorKernelSize ) { _edgeDetectorKernelSize = edgeDetectorKernelSize; }

double sb::Params::getEdgeDetectorLowThresh() const { return _edgeDetectorLowThresh; }

void sb::Params::setEdgeDetectorLowThresh( double edgeDetectorLowThresh ) { _edgeDetectorLowThresh = edgeDetectorLowThresh; }

double sb::Params::getEdgeDetectorHighThresh() const { return _edgeDetectorHighThresh; }

void sb::Params::setEdgeDetectorHighThresh( double edgeDetectorHighThresh ) { _edgeDetectorHighThresh = edgeDetectorHighThresh; }

double sb::Params::getEdgeDetectorBinarizeThresh() const { return _edgeDetectorBinarizeThresh; }

void sb::Params::setEdgeDetectorBinarizeThresh( double edgeDetectorBinarizeThresh ) { _edgeDetectorBinarizeThresh = edgeDetectorBinarizeThresh; }

double sb::Params::getEdgeDetectorBinarizeMaxValue() const { return _edgeDetectorBinarizeMaxValue; }

void sb::Params::setEdgeDetectorBinarizeMaxValue( double edgeDetectorBinarizeMaxValue ) { _edgeDetectorBinarizeMaxValue = edgeDetectorBinarizeMaxValue; }

double sb::Params::getHoughLinesPRho() const { return _houghLinesPRho; }

void sb::Params::setHoughLinesPRho( double houghLinesPRho ) { _houghLinesPRho = houghLinesPRho; }

double sb::Params::getHoughLinesPTheta() const { return _houghLinesPTheta; }

void sb::Params::setHoughLinesPTheta( double houghLinesPTheta ) { _houghLinesPTheta = houghLinesPTheta; }

int sb::Params::getHoughLinesPThreshold() const { return _houghLinesPThreshold; }

void sb::Params::setHoughLinesPThreshold( int houghLinesPThreshold ) { _houghLinesPThreshold = houghLinesPThreshold; }

double sb::Params::getHoughLinesPMinLineLength() const { return _houghLinesPMinLineLength; }

void sb::Params::setHoughLinesPMinLineLength( double houghLinesPMinLineLength ) { _houghLinesPMinLineLength = houghLinesPMinLineLength; }

double sb::Params::getHoughLinesPMaxLineGap() const { return _houghLinesPMaxLineGap; }

void sb::Params::setHoughLinesPMaxLineGap( double houghLinesPMaxLineGap ) { _houghLinesPMaxLineGap = houghLinesPMaxLineGap; }

void sb::Params::write( cv::FileStorage& fs ) const
{
	fs
			<< "{"

			<< "AreaToCrop" << _areaToCrop
			<< "RatioToSplit" << _ratioToSplit

			<< "EdgeDetectorKernelSize" << _edgeDetectorKernelSize
			<< "EdgeDetectorLowThresh" << _edgeDetectorLowThresh
			<< "EdgeDetectorHighThresh" << _edgeDetectorHighThresh
			<< "EdgeDetectorBinarizeThresh" << _edgeDetectorBinarizeThresh
			<< "EdgeDetectorBinarizeMaxValue" << _edgeDetectorBinarizeMaxValue

			<< "HoughLinesPRho" << _houghLinesPRho
			<< "HoughLinesPTheta" << _houghLinesPTheta
			<< "HoughLinesPThreshold" << _houghLinesPThreshold
			<< "HoughLinesPMinLineLength" << _houghLinesPMinLineLength
			<< "HoughLinesPMaxLineGap" << _houghLinesPMaxLineGap

			<< "}";
}

void sb::Params::read( const cv::FileNode& node )
{
	node["AreaToCrop"] >> _areaToCrop;
	node["RatioToSplit"] >> _ratioToSplit;

	node["EdgeDetectorKernelSize"] >> _edgeDetectorKernelSize;
	node["EdgeDetectorLowThresh"] >> _edgeDetectorLowThresh;
	node["EdgeDetectorHighThresh"] >> _edgeDetectorHighThresh;
	node["EdgeDetectorBinarizeThresh"] >> _edgeDetectorBinarizeThresh;
	node["EdgeDetectorBinarizeMaxValue"] >> _edgeDetectorBinarizeMaxValue;

	node["HoughLinesPRho"] >> _houghLinesPRho;
	node["HoughLinesPTheta"] >> _houghLinesPTheta;
	node["HoughLinesPThreshold"] >> _houghLinesPThreshold;
	node["HoughLinesPMinLineLength"] >> _houghLinesPMinLineLength;
	node["HoughLinesPMaxLineGap"] >> _houghLinesPMaxLineGap;
}

void sb::write( cv::FileStorage& fs, const std::string&, const sb::Params& data )
{
	data.write( fs );
}

void sb::read( const cv::FileNode& node, sb::Params& data, const sb::Params& defaultData )
{
	if ( node.empty() ) data = defaultData;
	else data.read( node );
}
