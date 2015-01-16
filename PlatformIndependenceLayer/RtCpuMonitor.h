/*
===============================================================================

	ReflecTech
	==========
	File		:	RtCpuMonitor.h
	Author		:	Jamie Taylor
	Last Edit	:	16/03/12
	Desc		:	Choose which cpu monitor implementation to load based on platform.

===============================================================================
*/


#ifndef RT_CPU_MONITOR_H
#define RT_CPU_MONITOR_H


#if RT_PLATFORM == RT_PLATFORM_WINDOWS
#include "Windows/RtCpuMonitorWin.h"
#elif RT_PLATFORM == RT_PLATFORM_LINUX
	#include "Linux/RtCpuMonitorLin.h"
#endif


#endif // RT_CPU_MONITOR_H