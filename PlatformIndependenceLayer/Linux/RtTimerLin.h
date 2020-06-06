/*
    Reflec-Tech
    -----------
    File        :   Linux/Timer.h
    Author      :   Jamie Taylor
    Last Edit   :   16/03/12
    Desc        :   Linux high-res timer header.
*/
#ifndef TIMERL_H
#define TIMERL_H


#include "../RtTimer.h" // for #include "Platform.h" -> define U32
//#include <time.h> // clock() - needed by all implementations
#include <sys/time.h> // gettimeofday()


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
    U32 GetMilliseconds();
    U32 GetMicroseconds();

    // identical across all implementations
    U32 GetMillisecondsCPU();
    U32 GetMicrosecondsCPU();

private:
    clock_t mStartTimeCPU;

    // time val holds elapsed seconds 
    // and the rest of elapsed time (fraction of a second) in microseconds
    timeval mStartTime;
};
/**************************************************************************************************************************/


#endif // TIMERL_H
