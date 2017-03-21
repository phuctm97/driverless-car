//#include "BoundariesJudge.h"
//#include "LineDetector.h"
//
//void sb::BoundariesJudge::apply( std::vector<sb::Section>& sections )
//{
//	filterLines( sections );
//
//	fillAndTranslateLines( sections );
//
//	calculateNops( sections );
//}
//
//void sb::BoundariesJudge::filterLines( std::vector<sb::Section>& sections )
//{
//	for ( int i = 0; i < _hypos->getNumberOfSections(); i++ ) {
//		filterLine( sections, i );
//	}
//}
//
//void sb::BoundariesJudge::filterLine( std::vector<sb::Section>& sections, int index )
//{
//	sb::Section& section = sections[index];
//
//	// 1.1) get references to lines array & reset them
//	std::vector<sb::Line>& leftLines = section.getLeftLines();
//	std::vector<sb::Line>& rightLines = section.getRightLines();
//
//	// 1.2) get section top and bottom line
//	const sb::Line topLine = section.getTopLine();
//	const sb::Line bottomLine = section.getBottomLine();
//
//	// 1.3) get hypothesises
//	const sb::Line& hypoM = _hypos->getMiddleLines()[index]; // hypo middle line
//
//	// 1.4) calculate knot point at middle bottom position
//	cv::Point2d pM;
//	sb::Line::findIntersection( hypoM, bottomLine, pM );
//
//	// 2.1) left lines
//	if ( !leftLines.empty() ) {
//
//		if ( allLinesAreOuter( leftLines ) ) {
//			keepOnlyLongestLine( leftLines );
//		}
//		else {
//			leftLines.erase( std::remove_if( leftLines.begin(),
//			                                 leftLines.end(),
//			                                 []( const sb::Line& l )-> bool {
//				                                 return !l.isInnerBoundary();
//			                                 } ),
//			                 leftLines.end() );
//		}
//
//		keepOnlyNearestLine( leftLines, bottomLine, hypoM, pM );
//
//	}
//	// 2.2) right lines
//	if ( !rightLines.empty() ) {
//
//		if ( allLinesAreOuter( rightLines ) ) {
//			keepOnlyLongestLine( rightLines );
//		}
//		else {
//			rightLines.erase( std::remove_if( rightLines.begin(),
//			                                  rightLines.end(),
//			                                  []( const sb::Line& l )-> bool {
//				                                  return !l.isInnerBoundary();
//			                                  } ),
//			                  rightLines.end() );
//		}
//
//		keepOnlyNearestLine( rightLines, bottomLine, hypoM, pM );
//	}
//}
//
//void sb::BoundariesJudge::fillAndTranslateLines( std::vector<Section>& sections ) const
//{
//	for ( int i = 0; i < _hypos->getNumberOfSections(); i++ ) {
//		fillAndTranslateLine( sections, i );
//	}
//}
//
//void sb::BoundariesJudge::fillAndTranslateLine( std::vector<Section>& sections, int index ) const
//{
//	sb::Section& section = sections[index];
//
//	// 1.1) get references to lines array & reset them
//	std::vector<sb::Line>& leftLines = section.getLeftLines();
//	std::vector<sb::Line>& rightLines = section.getRightLines();
//
//	// 1.2) get section top and bottom line
//	const sb::Line& topLine = section.getTopLine();
//	const sb::Line& bottomLine = section.getBottomLine();
//
//	// 1.3) get hypothesises
//	const sb::Line& hypoL = _hypos->getLeftLines()[index];
//	const sb::Line& hypoR = _hypos->getRightLines()[index];
//	const double laneWidth1 = _hypos->getLaneWidthMin() +
//			section.getAreaInContainer().y * _hypos->getLaneWidthRatio();
//	const double laneWidth2 = _hypos->getLaneWidthMin() +
//			(section.getAreaInContainer().y + section.getAreaInContainer().height) * _hypos->getLaneWidthRatio();
//
//	// copy / estimate line if there's no line
//	if ( leftLines.empty() ) {
//		// make sure that all sections always has a line
//
//		// line to copy
//		sb::Line line, lineToCopy, horizonLine;
//
//		// anchor point
//		cv::Point2d anchorPoint, topPoint, bottomPoint;
//
//		// angle to copy
//		double angleToCopy = 0;
//
//		if ( index == _hypos->getNumberOfSections() - 1 ) {
//
//			if ( !lineToCopy.isValid() &&
//				!sections[index - 1].getLeftLines().empty() &&
//				!sections[index - 1].getLeftLines().front().isEstimated() ) {
//				// copy directly + use hypo.angle and hypo.diff
//				lineToCopy = sections[index - 1].getLeftLines().front();
//				angleToCopy = _hypos->getAngleOfLeftLines()[index];
//				horizonLine = topLine;
//			}
//
//			if ( !lineToCopy.isValid() &&
//				_hypos->getDirection() == 0 ) {
//				// copy directly one of the nearest section
//				for ( int j = index - 1; j >= 0; j-- ) {
//					if ( !sections[j].getLeftLines().empty() && !sections[j].getLeftLines().front().isEstimated() ) {
//						lineToCopy = sections[j].getLeftLines().front();
//						angleToCopy = _hypos->getAngleOfLeftLines()[index];
//						horizonLine = topLine;
//						break;
//					}
//				}
//			}
//
//			if ( !lineToCopy.isValid() &&
//				!sections[index - 1].getLeftLines().empty() ) {
//				// copy directly + use hypo.angle and hypo.diff
//				lineToCopy = sections[index - 1].getLeftLines().front();
//				angleToCopy = _hypos->getAngleOfLeftLines()[index];
//				horizonLine = topLine;
//			}
//
//			if ( !lineToCopy.isValid() &&
//				_hypos->getDirection() == 0 ) {
//				// copy directly one of the nearest section
//				for ( int j = index - 1; j >= 0; j-- ) {
//					if ( !sections[j].getLeftLines().empty() ) {
//						lineToCopy = sections[j].getLeftLines().front();
//						angleToCopy = _hypos->getAngleOfLeftLines()[index];
//						horizonLine = topLine;
//						break;
//					}
//				}
//			}
//
//			if ( !lineToCopy.isValid() ) {
//				// use hypo.line
//				lineToCopy = _hypos->getLeftLines()[index];
//				lineToCopy.setInnerBoundary( true );
//				angleToCopy = _hypos->getAngleOfLeftLines()[index];
//				horizonLine = bottomLine;
//			}
//
//		}
//		else if ( index >= 1 ) {
//			if ( !lineToCopy.isValid() &&
//				!sections[index + 1].getLeftLines().empty() &&
//				!sections[index + 1].getLeftLines().front().isEstimated() ) {
//				// copy directly + use hypo.angle and hypo.diff
//				lineToCopy = sections[index + 1].getLeftLines().front();
//				angleToCopy = _hypos->getAngleOfLeftLines()[index];
//				horizonLine = bottomLine;
//			}
//
//			if ( !lineToCopy.isValid() &&
//				!sections[index - 1].getLeftLines().empty() &&
//				!sections[index - 1].getLeftLines().front().isEstimated() ) {
//				// copy directly + use hypo.angle and hypo.diff
//				lineToCopy = sections[index - 1].getLeftLines().front();
//				angleToCopy = _hypos->getAngleOfLeftLines()[index];
//				horizonLine = topLine;
//			}
//
//			if ( !lineToCopy.isValid() &&
//				!sections[index - 1].getLeftLines().empty() ) {
//				// compare the accuray of higher and lower line
//
//				sb::Line lowerLine = sections[index + 1].getLeftLines().front();
//				sb::Line higherLine = sections[index - 1].getLeftLines().front();
//
//				cv::Point2d pLow, pHigh, pHLow, pHHigh;
//				sb::Line::findIntersection( bottomLine, lowerLine, pLow );
//				sb::Line::findIntersection( bottomLine, _hypos->getLeftLines()[index + 1], pHLow );
//				sb::Line::findIntersection( topLine, higherLine, pHigh );
//				sb::Line::findIntersection( topLine, _hypos->getLeftLines()[index - 1], pHHigh );
//
//				if ( abs( pHHigh.x - pHigh.x ) <= (pHLow.x - pLow.x) ) {
//					lineToCopy = higherLine;
//					angleToCopy = _hypos->getAngleOfLeftLines()[index];
//					horizonLine = topLine;
//				}
//				else {
//					lineToCopy = lowerLine;
//					angleToCopy = _hypos->getAngleOfLeftLines()[index];
//					horizonLine = bottomLine;
//				}
//			}
//
//			if ( !lineToCopy.isValid() ) {
//				lineToCopy = sections[index + 1].getLeftLines().front();
//				angleToCopy = _hypos->getAngleOfLeftLines()[index];
//				horizonLine = bottomLine;
//			}
//
//		}
//		else {
//			lineToCopy = sections[index + 1].getLeftLines().front();
//			angleToCopy = _hypos->getAngleOfLeftLines()[index];
//			horizonLine = bottomLine;
//		}
//
//		sb::Line::findIntersection( horizonLine, lineToCopy, anchorPoint );
//
//		// * add some checks for angle, position and distance
//
//		// * add some modifications for angle and position
//
//		// line with previous angle and go through the anchor point
//		line = sb::Line( angleToCopy, anchorPoint );
//
//		// translate to current section
//		sb::Line::findIntersection( sb::Line( 0, 1, -(section.getAreaInContainer().y + section.getAreaInContainer().height * 0.75) ),
//		                            line, bottomPoint );
//		sb::Line::findIntersection( sb::Line( 0, 1, -(section.getAreaInContainer().y + section.getAreaInContainer().height * 0.25) ),
//		                            line, topPoint );
//		line = sb::Line( topPoint, bottomPoint );
//
//		// save the line
//		line.setInnerBoundary( lineToCopy.isInnerBoundary() );
//		line.setEstimated( true );
//		leftLines.push_back( line );
//	}
//
//	// confirm line
//	if ( !leftLines.empty() ) {
//
//		sb::Line& line = leftLines.front();
//
//		if ( line.isInnerBoundary() && line.isEstimated() && index < _hypos->getNumberOfSections() - 1 ) {
//			// middle or top section -> consider some criterias
//			cv::Point2d topPoint, bottomPoint, correctBottomPoint;
//			sb::Line::findIntersection( bottomLine, line, bottomPoint );
//			sb::Line::findIntersection( bottomLine, sections[index + 1].getLeftLines().front(), correctBottomPoint );
//
//			if ( abs( bottomPoint.x - correctBottomPoint.x ) > _params->getLineDetectorDistanceThreshold() ) {
//				// unacceptable line, translate to Pl and make a short line
//
//				double angle = _params->getLineDetectorAngleTradeOff() * line.getAngleWithOx()
//						+ (1 - _params->getLineDetectorAngleTradeOff()) * _hypos->getAngleOfLeftLines()[index];
//
//				line = sb::Line( angle, correctBottomPoint );
//				sb::Line::findIntersection( sb::Line( 0, 1, -(section.getAreaInContainer().y + section.getAreaInContainer().height * 0.7) ),
//				                            line, bottomPoint );
//				sb::Line::findIntersection( sb::Line( 0, 1, -(section.getAreaInContainer().y + section.getAreaInContainer().height * 0.3) ),
//				                            line, topPoint );
//				line = sb::Line( topPoint, bottomPoint );
//				line.setInnerBoundary( true );
//				line.setEstimated( true );
//			}
//		}
//		else if ( !line.isInnerBoundary() ) {
//			// outer line, translate and accept
//			cv::Point2d p1, p2;
//			sb::Line::findIntersection( topLine, line, p1 );
//			sb::Line::findIntersection( bottomLine, line, p2 );
//
//			line = sb::Line( p1 + cv::Point2d( laneWidth1, 0 ), p2 + cv::Point2d( laneWidth2, 0 ) );
//			line.setInnerBoundary( true );
//			line.setEstimated( true );
//		}
//	}
//
//	// copy/estimate line if there's no line
//	if ( rightLines.empty() ) {
//		// make sure that all sections always has a line
//
//		// line to copy
//		sb::Line line, lineToCopy, horizonLine;
//
//		// anchor point
//		cv::Point2d anchorPoint, topPoint, bottomPoint;
//
//		// angle to copy
//		double angleToCopy;
//
//		if ( index >= _hypos->getNumberOfSections() - 1 ) {
//			if ( !lineToCopy.isValid() &&
//				!sections[index - 1].getRightLines().empty() &&
//				!sections[index - 1].getRightLines().front().isEstimated() ) {
//				// copy directly + use hypo.angle and hypo.diff
//				lineToCopy = sections[index - 1].getRightLines().front();
//				angleToCopy = _hypos->getAngleOfRightLines()[index];
//				horizonLine = topLine;
//			}
//
//			if ( !lineToCopy.isValid() &&
//				_hypos->getDirection() == 0 ) {
//				// copy directly one of the nearest section
//				for ( int j = index - 1; j >= 0; j-- ) {
//					if ( !sections[j].getRightLines().empty() && !sections[j].getRightLines().front().isEstimated() ) {
//						lineToCopy = sections[j].getRightLines().front();
//						angleToCopy = _hypos->getAngleOfRightLines()[index];
//						horizonLine = topLine;
//						break;
//					}
//				}
//			}
//
//			if ( !lineToCopy.isValid() &&
//				!sections[index - 1].getRightLines().empty() ) {
//				// copy directly + use hypo.angle and hypo.diff
//				lineToCopy = sections[index - 1].getRightLines().front();
//				angleToCopy = _hypos->getAngleOfRightLines()[index];
//
//				horizonLine = topLine;
//			}
//
//			if ( !lineToCopy.isValid() &&
//				_hypos->getDirection() == 0 ) {
//				// copy directly one of the nearest section
//				for ( int j = index - 1; j >= 0; j-- ) {
//					if ( !sections[j].getRightLines().empty() ) {
//						lineToCopy = sections[j].getRightLines().front();
//						angleToCopy = _hypos->getAngleOfRightLines()[index];
//						horizonLine = topLine;
//						break;
//					}
//				}
//			}
//
//			if ( !lineToCopy.isValid() ) {
//				// use hypo.line
//				lineToCopy = _hypos->getRightLines()[index];
//				lineToCopy.setInnerBoundary( true );
//				angleToCopy = _hypos->getAngleOfRightLines()[index];
//				horizonLine = bottomLine;
//			}
//
//		}
//		else if ( index >= 1 ) {
//			if ( !lineToCopy.isValid() &&
//				!sections[index + 1].getRightLines().empty() &&
//				!sections[index + 1].getRightLines().front().isEstimated() ) {
//				// copy directly + use hypo.angle and hypo.diff
//				lineToCopy = sections[index + 1].getRightLines().front();
//				angleToCopy = _hypos->getAngleOfRightLines()[index];
//				horizonLine = bottomLine;
//			}
//
//			if ( !lineToCopy.isValid() &&
//				!sections[index - 1].getRightLines().empty() &&
//				!sections[index - 1].getRightLines().front().isEstimated() ) {
//				// copy directly + use hypo.angle and hypo.diff
//				lineToCopy = sections[index - 1].getRightLines().front();
//				angleToCopy = _hypos->getAngleOfRightLines()[index];
//				horizonLine = topLine;
//			}
//
//			if ( !lineToCopy.isValid() &&
//				!sections[index - 1].getRightLines().empty() ) {
//				// compare the accuray of higher and lower line
//
//				sb::Line lowerLine = sections[index + 1].getRightLines().front();
//				sb::Line higherLine = sections[index - 1].getRightLines().front();
//
//				cv::Point2d pLow, pHigh, pHLow, pHHigh;
//				sb::Line::findIntersection( bottomLine, lowerLine, pLow );
//				sb::Line::findIntersection( bottomLine, _hypos->getRightLines()[index + 1], pHLow );
//				sb::Line::findIntersection( topLine, higherLine, pHigh );
//				sb::Line::findIntersection( topLine, _hypos->getRightLines()[index - 1], pHHigh );
//
//				if ( abs( pHHigh.x - pHigh.x ) <= (pHLow.x - pLow.x) ) {
//					lineToCopy = higherLine;
//					angleToCopy = _hypos->getAngleOfRightLines()[index];
//					horizonLine = topLine;
//				}
//				else {
//					lineToCopy = lowerLine;
//					angleToCopy = _hypos->getAngleOfRightLines()[index];
//					horizonLine = bottomLine;
//				}
//			}
//
//			if ( !lineToCopy.isValid() ) {
//				lineToCopy = sections[index + 1].getRightLines().front();
//				angleToCopy = _hypos->getAngleOfRightLines()[index];
//				horizonLine = bottomLine;
//			}
//
//		}
//		else {
//			lineToCopy = sections[index + 1].getRightLines().front();
//			angleToCopy = _hypos->getAngleOfRightLines()[index];
//			horizonLine = bottomLine;
//		}
//
//		sb::Line::findIntersection( horizonLine, lineToCopy, anchorPoint );
//
//		// * add some checks for angle, position and distance
//
//		// * add some modifications for angle and position
//
//		// line with previous angle and go through the anchor point
//		line = sb::Line( angleToCopy, anchorPoint );
//
//		// translate to current section
//		sb::Line::findIntersection( sb::Line( 0, 1, -(section.getAreaInContainer().y + section.getAreaInContainer().height * 0.75) ),
//		                            line, bottomPoint );
//		sb::Line::findIntersection( sb::Line( 0, 1, -(section.getAreaInContainer().y + section.getAreaInContainer().height * 0.25) ),
//		                            line, topPoint );
//		line = sb::Line( topPoint, bottomPoint );
//
//		// save the line
//		line.setInnerBoundary( lineToCopy.isInnerBoundary() );
//		line.setEstimated( true );
//		rightLines.push_back( line );
//	}
//
//	// confirm line
//	if ( !rightLines.empty() ) {
//		sb::Line& line = rightLines.front();
//
//		if ( line.isInnerBoundary() && line.isEstimated() && index < _hypos->getNumberOfSections() - 1 ) {
//			// middle or top section -> consider some criterias
//			cv::Point2d topPoint, bottomPoint, correctBottomPoint;
//			sb::Line::findIntersection( bottomLine, line, bottomPoint );
//			sb::Line::findIntersection( bottomLine, sections[index + 1].getRightLines().front(), correctBottomPoint );
//
//			if ( abs( bottomPoint.x - correctBottomPoint.x ) > _params->getLineDetectorDistanceThreshold() ) {
//				// unacceptable line, translate to Pl and make a short line
//
//				double angle = _params->getLineDetectorAngleTradeOff() * line.getAngleWithOx()
//						+ (1 - _params->getLineDetectorAngleTradeOff()) * _hypos->getAngleOfRightLines()[index];
//
//				line = sb::Line( angle, correctBottomPoint );
//				sb::Line::findIntersection( sb::Line( 0, 1, -(section.getAreaInContainer().y + section.getAreaInContainer().height * 0.7) ),
//				                            line, bottomPoint );
//				sb::Line::findIntersection( sb::Line( 0, 1, -(section.getAreaInContainer().y + section.getAreaInContainer().height * 0.3) ),
//				                            line, topPoint );
//				line = sb::Line( topPoint, bottomPoint );
//				line.setInnerBoundary( true );
//				line.setEstimated( true );
//			}
//		}
//		else if ( !line.isInnerBoundary() ) {
//			// outer line, translate and accept
//			cv::Point2d p1, p2;
//			sb::Line::findIntersection( topLine, line, p1 );
//			sb::Line::findIntersection( bottomLine, line, p2 );
//
//			line = sb::Line( p1 - cv::Point2d( laneWidth1, 0 ), p2 - cv::Point2d( laneWidth2, 0 ) );
//			line.setInnerBoundary( true );
//			line.setEstimated( true );
//		}
//	}
//}
//
//void sb::BoundariesJudge::calculateNops( std::vector<sb::Section>& sections )
//{
//	const int N_SECTIONS = static_cast<int>(sections.size());
//
//	const double minX = _params->getMinX() - _hypos->getWorkingImageArea().x;
//	const double maxX = _params->getMaxX() - _hypos->getWorkingImageArea().x;
//
//	_Pl.clear();
//	_Pl.assign( N_SECTIONS + 1, cv::Point2d() );
//	_Pm.clear();
//	_Pm.assign( N_SECTIONS + 1, cv::Point2d() );
//	_Pr.clear();
//	_Pr.assign( N_SECTIONS + 1, cv::Point2d() );
//
//	for ( int i = -1; i < N_SECTIONS; i++ ) {
//		const sb::Line& bottomLine = i < 0
//			                             ? sb::Line( cv::Point( 0, 0 ), cv::Point( 1, 0 ) )
//			                             : sections[i].getBottomLine();
//
//		// left point
//		{
//			double higherVote = 0, lowerVote = 0;
//			sb::Line higherLine, lowerLine;
//
//			higherLine = sections[MAX( i, 0 )].getLeftLines().front();
//			if ( i < N_SECTIONS - 1 ) lowerLine = sections[i + 1].getLeftLines().front();
//
//			if ( higherLine.isValid() ) {
//				higherVote += 100000;
//				higherVote -= (bottomLine.getStartingPoint().y - higherLine.getEndingPoint().y);
//
//				if ( !higherLine.isEstimated() ) higherVote += 100000;
//			}
//			if ( lowerLine.isValid() ) {
//				lowerVote += 100000;
//				lowerVote -= (lowerLine.getStartingPoint().y - bottomLine.getEndingPoint().y);
//
//				if ( !lowerLine.isEstimated() ) lowerVote += 100000;
//			}
//
//			if ( higherVote > lowerVote ) {
//				// the higher line is considered as the better choice
//				sb::Line::findIntersection( bottomLine, higherLine, _Pl[i + 1] );
//			}
//			else {
//				// the lower line is considered as the better choice
//				sb::Line::findIntersection( bottomLine, lowerLine, _Pl[i + 1] );
//			}
//		}
//
//		// right point
//		{
//			double higherVote = 0, lowerVote = 0;
//			sb::Line higherLine, lowerLine;
//
//			higherLine = sections[MAX( i, 0 )].getRightLines().front();
//			if ( i < N_SECTIONS - 1 ) lowerLine = sections[i + 1].getRightLines().front();
//
//			if ( higherLine.isValid() ) {
//				higherVote += 100000;
//				higherVote -= (bottomLine.getStartingPoint().y - higherLine.getEndingPoint().y);
//
//				if ( !higherLine.isEstimated() ) higherVote += 100000;
//			}
//			if ( lowerLine.isValid() ) {
//				lowerVote += 100000;
//				lowerVote -= (lowerLine.getStartingPoint().y - bottomLine.getEndingPoint().y);
//
//				if ( !lowerLine.isEstimated() ) lowerVote += 100000;
//			}
//
//			if ( higherVote > lowerVote ) {
//				// the higher line is considered as the better choice
//				sb::Line::findIntersection( bottomLine, higherLine, _Pr[i + 1] );
//			}
//			else {
//				// the lower line is considered as the better choice
//				sb::Line::findIntersection( bottomLine, lowerLine, _Pr[i + 1] );
//			}
//		}
//
//		_Pl[i + 1].x = MIN( MAX( _Pl[i + 1].x, minX ), maxX );
//		_Pr[i + 1].x = MIN( MAX( _Pr[i + 1].x, minX ), maxX );
//		_Pm[i + 1] = (_Pl[i + 1] + _Pr[i + 1]) * 0.5;
//	}
//}
//
//bool sb::BoundariesJudge::allLinesAreOuter( const std::vector<sb::Line>& lines )
//{
//	bool allAreOuter = true;
//	for ( const sb::Line& l : lines )
//		if ( l.isInnerBoundary() ) {
//			allAreOuter = false;
//			break;
//		}
//
//	return allAreOuter;
//}
//
//void sb::BoundariesJudge::keepOnlyLongestLine( std::vector<sb::Line>& lines ) const
//{
//	sb::Line longestLine;
//	double maxVote = 0;
//
//	for ( const sb::Line& l : lines ) {
//		double len = abs( l.getStartingPoint().y - l.getEndingPoint().y );
//		if ( len > maxVote ) {
//			longestLine = l;
//			maxVote = len;
//		}
//	}
//
//	lines.clear();
//
//	lines.push_back( longestLine );
//}
//
//void sb::BoundariesJudge::keepOnlyNearestLine( std::vector<sb::Line>& leftLines,
//                                              const sb::Line& bottomLine,
//                                              const sb::Line& hypoM,
//                                              const cv::Point2d& pM ) const
//{
//	// keep only the nearest line 
//	sb::Line nearestLine;
//	double minVote = DBL_MAX;
//
//	for ( const sb::Line& l : leftLines ) {
//		double dis = hypoM.getDistance( l.getMiddlePoint() );
//
//		cv::Point2d p;
//		sb::Line::findIntersection( l, bottomLine, p );
//
//		double vote = _params->getLineDetectorDistanceTradeOff() * dis
//				+ (1 - _params->getLineDetectorDistanceTradeOff()) * abs( p.x - pM.x );
//
//		if ( vote < minVote ) {
//			nearestLine = l;
//			minVote = vote;
//		}
//	}
//
//	leftLines.clear();
//
//	leftLines.push_back( nearestLine );
//}
//
//const std::vector<cv::Point2d>& sb::BoundariesJudge::getAllPl() const
//{ return _Pl; }
//
//const std::vector<cv::Point2d>& sb::BoundariesJudge::getAllPm() const
//{ return _Pm; }
//
//const std::vector<cv::Point2d>& sb::BoundariesJudge::getAllPr() const
//{ return _Pr; }