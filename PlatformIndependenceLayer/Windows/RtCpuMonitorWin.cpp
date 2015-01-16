/*
===============================================================================

	ReflecTech
	==========
	File		:	RtCpuMonitorWin.cpp
	Author		:	Jamie Taylor
	Last Edit	:	07/07/13
	Desc		:	Basic class to monitor and report cpu usage as a percentage %,
					Windows implementation.

===============================================================================
*/


#include "RtCpuMonitorWin.h"


/*
================
CpuMonitor::CpuMonitor
================
*/
CpuMonitor::CpuMonitor( void ) {
	// ...
}

/*
================
CpuMonitor::~CpuMonitor
================
*/
CpuMonitor::~CpuMonitor( void ) {
	// ...
}

/*
================
CpuMonitor::Startup
================
*/
void CpuMonitor::Startup( void ) {
	PDH_STATUS status;

	lastPollTime = 0;
	cpuLoad = 0;

	canPollCpu = false;
	// create query object to poll cpu
	status = PdhOpenQuery( NULL, NULL, &queryHandle );
	if( status == ERROR_SUCCESS ) {
		// query all cpus in the system
		status = PdhAddCounter( queryHandle, TEXT( "\\Processor(_Total)\\% processor time" ), NULL, &counterHandle );
		if( status == ERROR_SUCCESS ) {
			canPollCpu = true;
			lastPollTime = timer.GetMilliseconds( );
		}
	}
}

/*
================
CpuMonitor::Shutdown
================
*/
void CpuMonitor::Shutdown( void ) {
	if( canPollCpu == true ) {
		PdhCloseQuery( queryHandle );
	}
}

/*
================
CpuMonitor::Update
================
*/
void CpuMonitor::Update( void ) {
	if( canPollCpu == true ) {
		if(  timer.GetMillisecondsCPU( ) >= ( lastPollTime + 1000 ) ) {
			PDH_FMT_COUNTERVALUE value;

			PdhCollectQueryData( queryHandle );
			PdhGetFormattedCounterValue( counterHandle, PDH_FMT_LONG, NULL, &value );

			cpuLoad = value.longValue;
			lastPollTime = timer.GetMillisecondsCPU( );
		}
	}
}

/*
================
CpuMonitor::GetCpuLoad
================
*/
U32 CpuMonitor::GetCpuLoad( void ) const {
	return static_cast<U32>( cpuLoad );
}