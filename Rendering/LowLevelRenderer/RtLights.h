/*
===============================================================================

    Reflec-Tech
    ===========
    File        :    RtLights.h
    Author      :    Jamie Taylor
    Last Edit   :    24/07/13
    Desc        :    Describes the kinds of the lights the engine supports.

===============================================================================
*/


#ifndef RT_LIGHTS_H
#define RT_LIGHTS_H


#include "../../PlatformIndependenceLayer/RtPlatform.h"
#include "../../CoreSystems/RtMemoryCommon.h"


struct DirectionalLight {
        DirectionalLight( void ) {
            ZeroMem( this, sizeof( this ) );
        }

    F32 ambientColour[4];
    F32 diffuseColour[4];

    F32 specularColour[4];
    F32 specularPower;

    F32 direction[3];
    //F32 padding;
};


#endif // RT_LIGHTS_H
