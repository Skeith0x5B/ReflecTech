/*
===============================================================================

    ReflecTech
    ==========
    File        :    RtGeoPrimtiveGenerator.h
    Author      :    Jamie Taylor
    Last Edit   :    29/06/13
    Desc        :    Class used to generate a handful of common geometric primitives.
                     Cubes, grids, cylinders and spheres are supported.

===============================================================================
*/


#ifndef RT_GEOMETRIC_PRIMITIVE_GENERATOR_H
#define RT_GEOMETRIC_PRIMITIVE_GENERATOR_H


#include "../../PlatformIndependenceLayer/RtPlatform.h"
#include "../../CoreSystems/RtAssert.h"

#include "../../CoreSystems/RtHeapAllocator.h"

#include "RtMesh.h"


/*
===============================================================================

Geo(metric) Primitive Generator class

===============================================================================
*/
class GeoPrimitiveGenerator {
public:
                                   GeoPrimitiveGenerator( void );
                                   ~GeoPrimitiveGenerator( void );

    void                           GenerateCube( U32 unitsAlongX, U32 unitsAlongY, U32 unitsAlongZ, Mesh &mesh );
                                   // useWorldOrigin means the box will use the origin of world space, this can be turned off
                                   // to ensure a bounding box aligns with a mesh for example
    void                           GenerateCubeF( F32 unitsAlongX, F32 unitsAlongY, F32 unitsAlongZ, Mesh &mesh, bool useWorldOrigin );
    void                           GenerateGrid( U32 unitsAlongX, U32 unitsAlongZ, U32 columns, U32 rows, Mesh &mesh );

private:
    HeapAllocator<void>            allocator;

                                   GeoPrimitiveGenerator( const GeoPrimitiveGenerator &ref ) { /* do nothing - forbidden op */ }
    GeoPrimitiveGenerator         & operator=( const GeoPrimitiveGenerator &rhs ) { /* do nothing - forbidden op */ }
};


#endif // RT_GEOMETRIC_PRIMITIVE_GENERATOR_H
