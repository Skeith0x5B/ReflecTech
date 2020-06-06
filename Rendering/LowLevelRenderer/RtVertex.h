/*
===============================================================================

    ReflecTech
    ==========
    File        :    RtVertex.h
    Author      :    Jamie Taylor
    Last Edit   :    27/06/13
    Desc        :    Basic vertex format, describes the properties of a vertex.
                     I'm likely going to go with the "uberformat" 
                     described in engine book page 415.

===============================================================================
*/


#ifndef RT_VERTEX_H
#define RT_VERTEX_H


#include "../../PlatformIndependenceLayer/RtPlatform.h"
//#include "../../CoreSystems/RtMemoryCommon.h"


/*
===============================================================================

Vertex format structure

Replace arrays with vectors once they're ready.

===============================================================================
*/
struct Vertex {
        //Vertex( void ) { 
        //    ZeroMemory( reinterpret_cast<void*>( &position[0] ), sizeof( F32 ) * 3  ); 
        //    ZeroMemory( reinterpret_cast<void*>( &normal[0] ), sizeof( F32 ) * 3  );
        //    diffuseColour[3] = 1.0f; 
        //}

        Vertex( F32 posX, F32 posY, F32 posZ, F32 nrmX, F32 nrmY, F32 nrmZ, F32 r, F32 g, F32 b, F32 a ) {
            position[0] = posX; position[1] = posY; position[2] = posZ;
            normal[0] = nrmX; normal[1] = nrmY; normal[2] = nrmZ;
            diffuseColour[0] = r; diffuseColour[1] = g; diffuseColour[2] = b; diffuseColour[3] = a;

            // added for lighting?
            //ZeroMemory( reinterpret_cast<void*>( &biNormal[0] ), sizeof( F32 ) * 3 );
            //ZeroMemory( reinterpret_cast<void*>( &tangent[0] ), sizeof( F32 ) * 3 );
        }

    F32 position[3];
    F32 normal[3];

    F32 diffuseColour[4];
    // added for texture mapping
    F32 textureCoordinates[3];
    
    // added for lighting?
    //F32 biNormal[3];
    //F32 tangent[3];

    // - add these attributes?
    // specular colour
    // ambient colour
};


#endif // RT_VERTEX_H
