/*
	Reflec-Tech
	-----------
	File		:	Windows/Timer.h
	Author		:	Jamie Taylor
	Last Edit	:	16/03/12
	Desc		:	Windows high-res timer implementation.
*/
#include "RtTimerWin.h"


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
	QueryPerformanceCounter(&mStartTime);
	QueryPerformanceFrequency(&mFrequency);
}
/**************************************************************************************************************************/

// May need a fix for possible QueryPerformanceCounter() jumps KB:274323
unsigned __int64 Timer::GetMilliseconds()
{
	LARGE_INTEGER current;
	QueryPerformanceCounter(&current);

	// elapsed milliseconds = (counter delta/freq)*1000
	return static_cast<unsigned __int64>((current.QuadPart - mStartTime.QuadPart) / (mFrequency.QuadPart / 1000.0f));
}
/**************************************************************************************************************************/

// May need a fix for possible QueryPerformanceCounter() jumps KB:274323
unsigned int Timer::GetMicroseconds()
{
	return 0;
}
/**************************************************************************************************************************/

// May need a fix for possible QueryPerformanceCounter() jumps KB:274323
float Timer::GetMillisecondsF()
{
	return 0.0;
}
/**************************************************************************************************************************/

// May need a fix for possible QueryPerformanceCounter() jumps KB:274323
float Timer::GetMicrosecondsF()
{
	return 0.0;
}
/**************************************************************************************************************************/

// identical across all implementations
unsigned __int64 Timer::GetMillisecondsCPU()
{
	// 1000 ms in a second
	clock_t currentTimeCPU = clock();
	unsigned __int64 delta = static_cast<__int64>( ((currentTimeCPU - mStartTimeCPU) / (CLOCKS_PER_SEC / 1000.0f)) );

	return delta;
}
/**************************************************************************************************************************/

unsigned __int64 Timer::GetMicrosecondsCPU()
{
	// 1000000 micros in a second
	clock_t currentTimeCPU = clock();
	unsigned __int64 delta = static_cast<__int64>( ((currentTimeCPU - mStartTimeCPU) / (CLOCKS_PER_SEC / 1000000.0f)) );

	return delta;
}
/**************************************************************************************************************************/