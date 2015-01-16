/*
===============================================================================

	ReflecTech
	==========
	File		:	RtStaticCamera.cpp, DSV = Doom Syntax Version
	Author		:	Jamie Taylor
	Last Edit	:	27/01/13
	Desc		:	A basic static camera class, specify a position, target.

                    TODO: Add camera frustum for possible frustum culling?

===============================================================================
*/


#include "RtStaticCamera.h"


/*
================
StaticCamera::StaticCamera
================
*/
StaticCamera::StaticCamera( void ) {
    cameraPosition = XMFLOAT3( 0.0f, 0.0f, 0.0f );
    cameraTarget   = XMFLOAT3( 0.0f, 0.0f, 1.0f );

    xBasisVector.x = 1.0f; xBasisVector.y = 0.0f; xBasisVector.z = 0.0f;
    yBasisVector.x = 0.0f; yBasisVector.y = 1.0f; yBasisVector.z = 0.0f;
    zBasisVector.x = 0.0f; zBasisVector.y = 0.0f; zBasisVector.z = 1.0f;

	viewMatrix  = XMMatrixIdentity();
	worldMatrix = XMMatrixIdentity();
}

/*
================
StaticCamera::StaticCamera
================
*/
StaticCamera::StaticCamera( const XMFLOAT3 *position, const XMFLOAT3 *target ) {
	cameraPosition = *position;
	cameraTarget = *target;
}

/*
================
StaticCamera::SetPosition
================
*/
void StaticCamera::SetPosition( const XMFLOAT3 *position ) {
	cameraPosition = *position;
}

/*
================
StaticCamera::SetTarget
================
*/
void StaticCamera::SetTarget( const XMFLOAT3 *target ) {
	cameraTarget = *target;
}

/*
================
StaticCamera::CalculateWorldToViewMatrix
================
*/
const XMMATRIX StaticCamera::CalculateViewMatrix( void ) {
	// create view matrix
    viewMatrix = XMMatrixLookAtLH( XMLoadFloat3( &cameraPosition ), XMLoadFloat3( &cameraTarget ), XMLoadFloat3( &yBasisVector ) );
  	return viewMatrix;
}

/*
================
StaticCamera::CalculateViewToWorldMatrix
================
*/
const XMMATRIX StaticCamera::CalculateWorldMatrix( void ) {
	// worldToView (aka view matrix) and viewToWorld are inverses of eachother
    worldMatrix = XMMatrixInverse( NULL, CalculateViewMatrix( ) );
    return worldMatrix;
}
