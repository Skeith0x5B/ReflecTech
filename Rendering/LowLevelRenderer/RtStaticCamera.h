/*
===============================================================================

	ReflecTech
	==========
	File		:	RtStaticCamera.h, DSV = Doom Syntax Version
	Author		:	Jamie Taylor
	Last Edit	:	26/01/13
	Desc		:	A basic static camera class, specify a position and target.

                    TODO: Add camera frustum for possible frustum culling?

===============================================================================
*/
#ifndef RT_STATIC_CAMERA_H
#define RT_STATIC_CAMERA_H


#include <D3D11.h> // needed for the XNA math types? - needed for compilation, can be removed when I start using my own math library
#include <xnamath.h> // replace with my own math library later on

#include "../../PlatformIndependenceLayer/RtPlatform.h"


/*
===============================================================================

StaticCamera class

===============================================================================
*/
class StaticCamera {
public:
					StaticCamera( void );
					StaticCamera( const XMFLOAT3 *position, const XMFLOAT3 *target );

	void			SetPosition( const XMFLOAT3 *position );
	void			SetTarget( const XMFLOAT3 *target );

    const XMMATRIX	CalculateViewMatrix( void );
    const XMMATRIX 	CalculateWorldMatrix( void );

private:
	XMFLOAT3		cameraPosition;
	XMFLOAT3		cameraTarget;

	XMFLOAT3		xBasisVector; 
	XMFLOAT3		yBasisVector;
	XMFLOAT3		zBasisVector;

	// TODO:		VIEW FRUSTUM

    XMMATRIX		viewMatrix;  // view matrix
    XMMATRIX		worldMatrix; // inverse of view matrix and vice versa
};
/*
===============================================================================
*/


#endif // RT_STATIC_CAMERA_H