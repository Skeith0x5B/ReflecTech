/*
	Reflec-Tech
	-----------
	File		:	RtWindows/Timer.h
	Author		:	Jamie Taylor
	Last Edit	:	16/03/12
	Desc		:	Windows high-res timer header.
*/
#ifndef RT_TIMER_WIN_H
#define RT_TIMER_WIN_H


//#include "../RtTimer.h"
#include <Windows.h>
#include <time.h> // needed by all implementations


/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
class Timer
{
public:
	Timer();
	~Timer();

	// reset the timer
	void Reset();

	// platform specific
	unsigned __int64 GetMilliseconds();
	unsigned int	 GetMicroseconds();
	float			 GetMillisecondsF();
	float			 GetMicrosecondsF();

	// identical across all implementations
	unsigned __int64 GetMillisecondsCPU();
	unsigned __int64 GetMicrosecondsCPU();
	
private:
	clock_t mStartTimeCPU;
	
	// LARGE_INTEGER for start 'time' and frequency
	LARGE_INTEGER mStartTime;
	LARGE_INTEGER mFrequency;
};
/**************************************************************************************************************************/


#endif // RT_TIMER_WIN_H
