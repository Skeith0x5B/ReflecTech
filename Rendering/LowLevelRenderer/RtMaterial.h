/*
===============================================================================

    ReflecTech
    ==========
    File        :    RtMaterial.h
    Author      :    Jamie Taylor
    Last Edit   :    09/08/13
    Desc        :    Basic material structure, describes a material where a material
                     is a description of the visual properties of a surface.

===============================================================================
*/


#ifndef RT_MATERIAL_H
#define RT_MATERIAL_H


#include "../../PlatformIndependenceLayer/RtPlatform.h"
#include "../../CoreSystems/RtMemoryCommon.h"


#define MAX_TEXTURE_FILENAME_STRING_LENGTH 64


/*
===============================================================================

Material structure

           Short  materialId;?
           Material hash for quick comparison &| location?

===============================================================================
*/
enum MATERIAL_RENDER_STATE {
    SOLID_LH       = 0,
    SOLID_RH       = 1,
    WIREFRAME_LH   = 2,
    WIREFRAME_RH   = 3,
    STENCIL_SHADOW = 4, // move this to depth/stencil state enum?
};

// use a namespace here as enum won't allow entries of any format other than
// basic integers
/*
namespace MATERIAL_COLOURS {
    // replace with vectors when math library is implemented
    F32 WHITE[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    F32 BLACK[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    F32 RED[4]   = { 1.0f, 0.0f, 0.0f, 1.0f };
    F32 GREEN[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
    F32 BLUE[4]  = { 0.0f, 0.0f, 1.0f, 1.0f };
};*/


/*
===============================================================================

Material structure

===============================================================================
*/

struct Material {
                            Material( void ) { 
                                ZeroMemory( this, sizeof( this ) ); 
                            }

    I8                      materialName[MAX_TEXTURE_FILENAME_STRING_LENGTH];
    MATERIAL_RENDER_STATE   renderState;


    F32                     ambientColour[3];
    F32                     diffuseColour[3];
    F32                     specularColour[3];
    F32                     specularCoefficient;

    void *                  diffuseMap;
    I8                      diffuseMapName[MAX_TEXTURE_FILENAME_STRING_LENGTH];
    void *                  normalMap;
    I8                      normalMapName[MAX_TEXTURE_FILENAME_STRING_LENGTH];
    void *                  specularMap;
    I8                      specularMapName[MAX_TEXTURE_FILENAME_STRING_LENGTH];
};


void SetToDefaultMaterial( Material *material );


#endif // RT_MATERIAL_H
