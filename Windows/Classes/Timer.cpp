#include "Timer.h"
#include <opencv2/core/affine.hpp>

int sb::Timer::reset( const std::string& key )
{
	if ( _recoredClocks.find( key ) != _recoredClocks.end() ) {
		_recoredClocks[key] = clock();
	}
	else {
		_recoredClocks.insert( std::pair<std::string, clock_t>( key, clock() ) );
	}

	return 0;
}

int sb::Timer::milliseconds( const std::string& key )
{
	if ( _recoredClocks.find( key ) != _recoredClocks.end() ) {
		return (clock() - _recoredClocks[key]) * 1000 / CLOCKS_PER_SEC;
	}

	return -1;
}

float sb::Timer::fps( const std::string& key )
{
	int time = milliseconds( key );

	if ( time < 0 )return -1;

	return 1000.0f / time;
}
