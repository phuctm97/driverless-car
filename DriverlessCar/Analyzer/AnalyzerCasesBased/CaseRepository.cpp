#include "CaseRepository.h"

bool sb::CaseRepository::empty() const { return _cases.empty(); }

const std::list<sb::ICase*>& sb::CaseRepository::getAll() const
{
	return _cases;
}

sb::ICase* sb::CaseRepository::last() const
{
	if( !empty() ) return _cases.front();
	else return nullptr;
}

void sb::CaseRepository::push( ICase* newCase )
{
	if( _cases.size() == _capacity ) {
		delete _cases.back();
		_cases.pop_back();
	}

	_cases.push_front( newCase );
}

void sb::CaseRepository::mapRoadWidth( int row, int width )
{
	auto it = _rowToRoadWidthMap.find( row );

	if( it == _rowToRoadWidthMap.end() ) {
		_rowToRoadWidthMap.emplace( row, width );
	}
	else {
		it->second = width;
	}
}

int sb::CaseRepository::findRoadWidth( int row )
{
	auto it = _rowToRoadWidthMap.find( row );
	if( it == _rowToRoadWidthMap.end() ) return -1;
	return it->second;
}

void sb::CaseRepository::release()
{
	for( ICase* caseToRelease : _cases ) {
		delete caseToRelease;
	}

	_cases.clear();
	_rowToRoadWidthMap.clear();
}
