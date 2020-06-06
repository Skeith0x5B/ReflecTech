/*
    Reflec-Tech
    -----------
    File        :    RtDynLib.h
    Author      :    Jamie Taylor
    Last Edit   :    04/09/12
    Desc        :    Represents a DLL/SO; can be used on its own or with the manager. 

                    _H - using heap allocator
                    _DP - using dynamic pool allocator
                    _V - volatile; source code likely/probable to change
*/
#ifndef RT_DYN_LIB_H
#define RT_DYN_LIB_H



#include "../RtCommonHeaders.h" // platform.h (and <iostream>(TEMP))


#if RT_PLATFORM == RT_PLATFORM_WINDOWS
    #include "Windows/RtDynamicLibraryWindows.h"
#elif RT_PLATFORM == RT_PLATFORM_LINUX
    #include "Linux/RtDynLibLin.h"
#endif


#endif // RT_DYN_LIB_H
