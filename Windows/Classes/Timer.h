#ifndef __SB_TIMER_H__
#define __SB_TIMER_H__

#include <map>
#include <ctime>
#include <iostream>

namespace sb
{

class Timer
{
private:
	std::map<std::string, clock_t> _recoredClocks;

public:
	Timer() {}

	int reset( const std::string& key );

	int milliseconds( const std::string& key );

	float fps( const std::string& key );
};

}


#endif //!__SB_TIMER_H__