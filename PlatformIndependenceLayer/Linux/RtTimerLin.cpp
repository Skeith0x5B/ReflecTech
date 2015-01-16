/*
	Reflec-Tech
	-----------
	File		:	Linux/Timer.h
	Author		:	Jamie Taylor
	Last Edit	:	16/03/12
	Desc		:	Linux high-res timer header.
*/
#include "TimerLin.h"


/**************************************************************************************************************************/
// Implementation 
/**************************************************************************************************************************/
Timer::Timer()
{
	Reset();
}
/**************************************************************************************************************************/

Timer::~Timer()
{
	// ...
}
/**************************************************************************************************************************/

void Timer::Reset()
{
	// reset the platform neutral variable
	mStartTimeCPU = clock();

	// platform specific resets
	gettimeofday(&mStartTime, 0);
}
/**************************************************************************************************************************/

U32 Timer::GetMilliseconds()
{
	timeval current;
	gettimeofday(&current, 0);
	// elapsed milliseconds = (delta seconds * 1000) + (delta microseconds / 1000) 
	return static_cast<U32>(((current.tv_sec - mStartTime.tv_sec)*1000) + ((current.tv_usec - mStartTime.tv_usec)/1000));
}
/**************************************************************************************************************************/

U32 Timer::GetMicroseconds()
{
	timeval current;
	gettimeofday(&current, 0);
	// elapsed microseconds = (delta seconds * 1000000) + (delta microseconds)
	return static_cast<U32>(((current.tv_sec - mStartTime.tv_sec)*1000000) + (current.tv_usec - mStartTime.tv_usec));
}
/**************************************************************************************************************************/

// identical across all implementations
U32 Timer::GetMillisecondsCPU()
{
	// 1000 ms in a second
	//clock_t newClock = clock();
	return static_cast<U32>(((float)clock() - mStartTimeCPU) / ((float)CLOCKS_PER_SEC / 1000.0));
}
/**************************************************************************************************************************/

U32 Timer::GetMicrosecondsCPU()
{
	// 1000000 micros in a second
	//clock_t newClock = clock();
	return static_cast<U32>(((float)clock() - mStartTimeCPU) / ((float)CLOCKS_PER_SEC / 1000000.0));
}
/**************************************************************************************************************************/
