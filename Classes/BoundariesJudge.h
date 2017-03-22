//#ifndef __SB_BOUNDARIES_JUDGE_H__
//#define __SB_BOUNDARIES_JUDGE_H__
//
//#include "../seboys-hypos-generator/Hypos.h"
//#include "../seboys-params-generator/Params.h"
//#include "Section.h"
//
//namespace sb
//{
//class BoundariesJudge
//{
//private:
//	sb::Hypos* _hypos;
//	sb::Params* _params;
//
//	std::vector<cv::Point2d> _Pl;
//	std::vector<cv::Point2d> _Pm;
//	std::vector<cv::Point2d> _Pr;
//
//public:
//	BoundariesJudge( sb::Hypos* hypos, sb::Params* params )
//		: _hypos( hypos ),
//		  _params( params ) {}
//
//	void apply( std::vector<sb::Section>& sections );
//
//	const std::vector<cv::Point2d>& getAllPl() const;
//
//	const std::vector<cv::Point2d>& getAllPm() const;
//
//	const std::vector<cv::Point2d>& getAllPr() const;
//
//private:
//	void filterLines( std::vector<sb::Section>& sections );
//
//	void filterLine( std::vector<sb::Section>& sections, int index );
//
//	void fillAndTranslateLines( std::vector<Section>& sections ) const;
//
//	void fillAndTranslateLine( std::vector<Section>& sections, int index ) const;
//
//	void calculateNops( std::vector<sb::Section>& sections );
//
//	void keepOnlyLongestLine( std::vector<sb::Line>& lines ) const;
//
//	void keepOnlyNearestLine( std::vector<sb::Line>& leftLines, 
//														const sb::Line& bottomLine, 
//														const sb::Line& hypoM,
//														const cv::Point2d& pM ) const;
//
//	static bool allLinesAreOuter( const std::vector<sb::Line>& lines );
//
//};
//}
//
//#endif //!__SB_BOUNDARIES_JUDGE_H__
