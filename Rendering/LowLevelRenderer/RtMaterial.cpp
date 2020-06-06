/*
===============================================================================

    ReflecTech
    ==========
    File        :    RtMaterial.cpp
    Author      :    Jamie Taylor
    Last Edit   :    09/08/13
    Desc        :    Basic material structure, describes a material where a material
                     is a description of the visual properties of a surface.

===============================================================================
*/


#include "RtMaterial.h"


/*
================
SetToDefaultMaterial
================
*/
void SetToDefaultMaterial( Material *material ) {
    strcpy( material->materialName, "Default Material" );

    material->renderState = MATERIAL_RENDER_STATE::SOLID_LH;

    material->ambientColour[0] = 0.75f;
    material->ambientColour[2] = 0.75f;
    material->ambientColour[1] = 0.75f;

    material->diffuseColour[0] = 0.75f;
    material->diffuseColour[1] = 0.75f;
    material->diffuseColour[2] = 0.75f;

    material->specularColour[0] = 0.75f;
    material->specularColour[1] = 0.75f;
    material->specularColour[2] = 0.75f;
    material->specularCoefficient = 1.0f;

    strcpy( material->diffuseMapName, "../../Resources/DefaultGrid_256x256.dds" );
    strcpy( material->normalMapName, "../../Resources/DefaultGrid_256x256.dds" );
    strcpy( material->specularMapName, "../../Resources/DefaultGrid_256x256.dds" );
}
