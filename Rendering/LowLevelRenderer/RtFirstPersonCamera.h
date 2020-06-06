/*
===============================================================================

    ReflecTech
    ==========
    File        :    RtFirstPersonCamera.h, DSV = Doom Syntax Version
    Author      :    Jamie Taylor
    Last Edit   :    28/01/13
    Desc        :    A basic first-person camera class.

                     TODO: - Add camera frustum for possible frustum culling?

===============================================================================
*/


#ifndef RT_FIRST_PERSON_CAMERA
#define RT_FIRST_PERSON_CAMERA


#include <D3D11.h> // needed for the XNA math types? - needed for compilation, can be removed when I start using my own math library
#include <xnamath.h> // replace with my own math library later on

#include "../../PlatformIndependenceLayer/RtPlatform.h"


#define INFINITE_PITCH -1.0f
#define INFINITE_YAW -1.0f


/*
===============================================================================

First-person camera class

===============================================================================
*/
class FirstPersonCamera {
public:
                    FirstPersonCamera( void );

    void            SetPosition( const XMFLOAT3 *position );
    void            SetTarget( const XMFLOAT3 *target );

    void            SetRotation( F32 _minPitch, F32 currentPitch, F32 _maxPitch, bool _infinitePitch, 
                                 F32 _minYaw, F32 currentYaw, F32 _maxYaw, bool _infiniteYaw);
    void            Rotate( F32 pitchDelta, F32 yawDelta );
    void            Translate( F32 xDelta, F32 yDelta, F32 zDelta );

    const XMMATRIX  CalculateWorldToViewMatrix( void );
    const XMMATRIX  CalculateViewToWorldMatrix( void );

private:
    XMFLOAT3        cameraPosition;
    XMFLOAT3        cameraTarget;

    F32             minPitch, pitch, maxPitch;
    F32             minYaw, yaw, maxYaw;
    bool            infinitePitch, infiniteYaw;

    // TODO:        VIEW FRUSTUM

    XMMATRIX        worldToView; // view matrix
    XMMATRIX        viewToWorld; // inverse of view matrix and vice versa
};
/*
===============================================================================
*/


#endif // RT_FIRST_PERSON_CAMERA
