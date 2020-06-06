/*
===============================================================================

    ReflecTech
    ==========
    File        :   RtCpuMonitorWin.h
    Author      :   Jamie Taylor
    Last Edit   :   07/07/13
    Desc        :   Basic class to monitor and report cpu usage as a percentage %,
                    Linux implementation.

===============================================================================
*/


#ifndef RT_CPU_MONITOR_WIN_H
#define RT_CPU_MONITOR_WIN_H


#include "../../PlatformIndependenceLayer/RtPlatform.h"
#include "../../PlatformIndependenceLayer/Linux/RtTimerLin.h"



/*
===============================================================================

Cpu monitor class

===============================================================================
*/
class CpuMonitor {
public:
            CpuMonitor( void );
            ~CpuMonitor( void );

    void    Startup( void );
    void    Shutdown( void );
    void    Update( void );
    U32     GetCpuLoad( void ) const;

private:
    bool     canPollCpu;
    U64      cpuLoad;

    Timer     timer;
    clock_t  lastPollTime;

    //HQUERY   queryHandle;
    //HCOUNTER counterHandle;
};


#endif // RT_CPU_MONITOR_H
