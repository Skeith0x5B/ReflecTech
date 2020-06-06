/*
===============================================================================

    ReflecTech
    ==========
    File        :    RtFrameCounter.cpp
    Author      :    Jamie Taylor
    Last Edit   :    07/07/13
    Desc        :    Basic class to count the frames elapsed in a one second time frame.

===============================================================================
*/


#include "RtFrameCounter.h"


/*
================
FrameCounter::FrameCounter
================
*/
FrameCounter::FrameCounter( void ) {
    // ...
}

/*
================
FrameCounter::~FrameCounter
================
*/
FrameCounter::~FrameCounter( void ) {
    // ...
}

/*
================
FrameCounter::Startup
================
*/
void FrameCounter::Startup( void ) {
    fps = maxFps = 0;
    minFps = U32_MAX;
    frameCount = 0;
    startTime = timer.GetMillisecondsCPU( );
}

/*
================
FrameCounter::Update
================
*/
void FrameCounter::Update( void ) {
    ++frameCount;

    // has a second elapsed?
    if( timer.GetMillisecondsCPU( ) >= ( startTime + 1000 ) ) {
        // set fps and reset internal state
        fps = frameCount;

        if( fps < minFps ) {
            minFps = fps;
        }

        if( fps > maxFps ) {
            maxFps = fps;
        }

        frameCount = 0;
        startTime  = timer.GetMillisecondsCPU( );
    }
}

/*
================
FrameCounter::GetFrameCount
================
*/
U32 FrameCounter::GetFrameCount( void ) const {
    return fps;
}

/*
================
FrameCounter::GetMinFrameCount
================
*/
U32 FrameCounter::GetMinFrameCount( void ) const {
    return minFps;
}

/*
================
FrameCounter::GetMaxFrameCount
================
*/
U32 FrameCounter::GetMaxFrameCount( void ) const {
    return maxFps;
}
