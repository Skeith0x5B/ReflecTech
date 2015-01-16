/*
===============================================================================

	ReflecTech
	==========
	File		:	RtFrameCounter.h
	Author		:	Jamie Taylor
	Last Edit	:	07/07/13
	Desc		:	Basic class to count the frames elapsed in a one second time frame.

===============================================================================
*/


#ifndef RT_FRAME_COUNTER_H
#define RT_FRAME_COUNTER_H


#include "../PlatformIndependenceLayer/RtPlatform.h"
#include "../PlatformIndependenceLayer/Windows/RtTimerWin.h"


/*
===============================================================================

Frame counter class

===============================================================================
*/
class FrameCounter {
public:
			FrameCounter( void );
			~FrameCounter( void );

	void	Startup( void );
	void	Update( void );
			// fps for this frame
	U32		GetFrameCount( void ) const;
			// the lowest fps recorded since the simulation began
	U32		GetMinFrameCount( void ) const;
			// the highest fps recorded since the simulation began
	U32		GetMaxFrameCount( void ) const;

private:
	U32		minFps, fps, maxFps;
	U32		frameCount;

	Timer   timer;
	clock_t startTime;
};


#endif // RT_FRAME_COUNTER_H