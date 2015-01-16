/*
===============================================================================

	ReflecTech
	==========
	File		:	Timer.h
	Author		:	Jamie Taylor
	Last Edit	:	16/03/12
	Desc		:	Choose which timer implementation to load.

===============================================================================
*/


#ifndef TIMER_H
#define TIMER_H


#include "RtPlatform.h"
#include <time.h> // needed by all implementations


#if RT_PLATFORM == RT_PLATFORM_WINDOWS
	#include "Windows/RtTimerWin.h"
#elif RT_PLATFORM == RT_PLATFORM_LINUX
	#include "Linux/RtTimerLin.h"
#endif


#endif // TIMER_H
