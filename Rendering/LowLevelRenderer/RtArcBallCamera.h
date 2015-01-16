/*
===============================================================================

	ReflecTech
	==========
	File		:	RtArcBallCamera.h, DSV = Doom Syntax Version
	Author		:	Jamie Taylor
	Last Edit	:	03/06/13
	Desc		:	A basic arc ball camera class, rotates the camera about an object.
					Used for things like model and level editors/viewers.

                    TODO: - Add camera frustum for possible frustum culling?
                    
                    Fixed crash bug by removing set position function, was setting
                    position instead of zoom, this resulted in 3 vectors each with a y
                    component of zero. This is likely what caused the LHLookAt call
                    to fail.	

===============================================================================
*/


#ifndef RT_ARC_BALL_CAMERA_H
#define RT_ARC_BALL_CAMERA_H


#include <D3D11.h> // needed for the XNA math types? - needed for compilation, can be removed when I start using my own math library
#include <xnamath.h> // replace with my own math library later on

#include "../../PlatformIndependenceLayer/RtPlatform.h"


#define INFINITE_PITCH -1.0f
#define INFINITE_YAW -1.0f


/*
===============================================================================

ArcBallCamera class

===============================================================================
*/
class ArcBallCamera {
public:
					ArcBallCamera( void );
					ArcBallCamera( const XMFLOAT3 *target );

	void			SetTarget( const XMFLOAT3 *target );

	void			SetZoom( F32 minZoom, F32 currentZoom, F32 maxZoom );
	void			Zoom( F32 zoomDelta );

	void 			SetRotation( F32 pitch, F32 yaw, F32 _minPitch, F32 _maxPitch );
	void			Rotate( F32 pitchDelta, F32 yawDelta );

    const XMMATRIX	CalculateViewMatrix( void );
    const XMMATRIX 	CalculateWorldMatrix( void );

	const XMFLOAT3  GetCameraPosition( void ) const;

private:
	XMFLOAT3		cameraPosition;
	XMFLOAT3		cameraTarget;

	XMFLOAT3		xBasisVector; 
	XMFLOAT3		yBasisVector;
	XMFLOAT3		zBasisVector; 

	F32				minPitch, pitch, maxPitch;
	F32				yaw;

	F32				minZoom, zoom, maxZoom;

	// TODO:		VIEW FRUSTUM

    XMMATRIX		viewMatrix;  // view matrix
    XMMATRIX		worldMatrix; // inverse of view matrix and vice versa
};
/*
===============================================================================
*/


#endif // RT_ARC_BALL_CAMERA
