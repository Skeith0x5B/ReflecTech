/*
===============================================================================

    ReflecTech
    ==========
    File        :   RtCpuMonitorLin.cpp
    Author      :   Jamie Taylor
    Last Edit   :   07/07/13
    Desc        :   Basic class to monitor and report cpu usage as a percentage %,
                    Linux implementation.

===============================================================================
*/


#include "RtCpuMonitorLin.h"


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
    // ...
}

/*
================
CpuMonitor::Shutdown
================
*/
void CpuMonitor::Shutdown( void ) {
    // ...
}

/*
================
CpuMonitor::Update
================
*/
void CpuMonitor::Update( void ) {
    // ...
}

/*
================
CpuMonitor::GetCpuLoad
================
*/
U32 CpuMonitor::GetCpuLoad( void ) const {
    return static_cast<U32>( cpuLoad );
}
