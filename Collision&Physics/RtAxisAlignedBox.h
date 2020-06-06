/*
===============================================================================

    ReflecTech
    ==========
    File        :   RtAxisAlignedBox.h
    Author      :   Jamie Taylor
    Last Edit   :   31/07/13
    Desc        :   Basic AABB (axis-aligned bounding box).
                    Using the minVector, maxVector representation due to simplicity
                    (though the origin + extents approach isn't difficult either), I'd
                    rather keep it simple for now as there's still plenty to get on with.

===============================================================================
*/


#ifndef RT_AXIS_ALIGNED_BOX_H
#define RT_AXIS_ALIGNED_BOX_H


#include "../PlatformIndependenceLayer/RtPlatform.h"


struct AxisAlignedBox {
    //AxisAlignedBox( void ) { ZeroMemory( this, sizeof( this ) ); }
    AxisAlignedBox( void ) { minX = maxX = centerX = 0.0f;
                             minY = maxY = centerY = 0.0f;
                             minZ = maxZ = centerZ = 0.0f;
    }


    F32 minX, maxX, centerX;
    F32 minY, maxY, centerY;
    F32 minZ, maxZ, centerZ;
};


#endif // RT_AXIS_ALIGNED_BOX_H
