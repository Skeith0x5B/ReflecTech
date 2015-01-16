/*
===============================================================================

	ReflecTech
	==========
	File		:	RtArcBallCamera.cpp, DSV = Doom Syntax Version
	Author		:	Jamie Taylor
	Last Edit	:	28/01/13
	Desc		:	A basic arc ball camera class, rotates the camera about an object.
					Used for things like model and level editors/viewers.

                    TODO: - Add camera frustum for possible frustum culling?

===============================================================================
*/


#include "RtArcBallCamera.h"


/*
================
ArcBallCamera::ArcBallCamera
================
*/
ArcBallCamera::ArcBallCamera( void ) {
    cameraPosition = XMFLOAT3( 0.0f, 0.0f, 0.0f );
    cameraTarget = XMFLOAT3( 0.0f, 0.0f, 1.0f );

    xBasisVector.x = 1.0f; xBasisVector.y = 0.0f; xBasisVector.z = 0.0f;
    yBasisVector.x = 0.0f; yBasisVector.y = 1.0f; yBasisVector.z = 0.0f;
    zBasisVector.x = 0.0f; zBasisVector.y = 0.0f; zBasisVector.z = 1.0f;

	pitch = yaw = 0.0f;

	zoom = 0.0f;

	viewMatrix  = XMMatrixIdentity( );
	worldMatrix = XMMatrixIdentity( );
}

/*
================
ArcBallCamera::ArcBallCamera
================
*/
ArcBallCamera::ArcBallCamera( const XMFLOAT3 *target ) {
	//cameraPosition = XMFLOAT3( 0.0f, 0.0f, 0.0f );
    cameraTarget = *target;

    xBasisVector.x = 1.0f; xBasisVector.y = 0.0f; xBasisVector.z = 0.0f;
    yBasisVector.x = 0.0f; yBasisVector.y = 1.0f; yBasisVector.z = 0.0f;
    zBasisVector.x = 0.0f; zBasisVector.y = 0.0f; zBasisVector.z = 1.0f;

	minPitch = -1.5707963267f;
	maxPitch = 1.5707963267f;
	pitch = yaw = 0.0f;

	zoom = 0.0f;

	viewMatrix  = XMMatrixIdentity( );
	worldMatrix = XMMatrixIdentity( );
}

/*
================
ArcBallCamera::SetTarget
================
*/
void ArcBallCamera::SetTarget( const XMFLOAT3 *target ) {
	cameraTarget = *target;
}

/*
================
ArcBallCamera::SetZoomLimits
================
*/
void ArcBallCamera::SetZoom( F32 _minZoom, F32 currentZoom, F32 _maxZoom ) {
	minZoom = _minZoom;
	zoom = currentZoom;
	maxZoom = _maxZoom;

	if( zoom > maxZoom ) {
		zoom = maxZoom;
	} else if( zoom < minZoom ) {
		zoom = minZoom;
	}
}

/*
================
ArcBallCamera::Zoom
================
*/
void ArcBallCamera::Zoom( F32 zoomDelta ) {
	zoom += zoomDelta;

	if( zoom > maxZoom ) {
		zoom = maxZoom;
	} else if( zoom < minZoom ) {
		zoom = minZoom;
	}
}

/*
================
ArcBallCamera::SetRotation
================
*/
void ArcBallCamera::SetRotation( F32 pitch_, F32 yaw_, F32 minPitch_, F32 maxPitch_ ) {
	minPitch = minPitch_;
	pitch = pitch_;
	maxPitch = maxPitch_;

	yaw = yaw_;

	if( ( pitch > maxPitch ) ) {
		pitch = maxPitch;
	} else if( ( pitch < minPitch ) ) {
		pitch = minPitch;
	}
}

/*
================
ArcBallCamera::Rotate
================
*/
void ArcBallCamera::Rotate( F32 pitchDelta, F32 yawDelta ) {
	pitch += pitchDelta;
	yaw	+= yawDelta;

	if( ( pitch > maxPitch ) ) {
		pitch = maxPitch;
	} else if( ( pitch < minPitch ) ) {
		pitch = minPitch;
	}
}

/*
================
ArcBallCamera::CalculateWorldToViewMatrix
================
*/
const XMMATRIX ArcBallCamera::CalculateViewMatrix( void ) {
	// create view matrix
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw( pitch, yaw, 0.0f );
	XMVECTOR zReference = XMVectorSet( 0.0f, 0.0f, zoom, 1.0f );
	XMVECTOR newUp = XMLoadFloat3( &yBasisVector );

	zReference = XMVector3Transform( zReference, rotation );
	newUp = XMVector3Transform( newUp, rotation );

	XMVECTOR position = XMLoadFloat3( &cameraPosition );
	XMVECTOR target = XMLoadFloat3( &cameraTarget );
	position = target + zReference;
	XMStoreFloat3( &cameraPosition, position );

	viewMatrix = XMMatrixLookAtLH( position, target, newUp  );

  	return viewMatrix;
}

/*
================
ArcBallCamera::CalculateViewToWorldMatrix
================
*/
const XMMATRIX ArcBallCamera::CalculateWorldMatrix( void ) {
	// worldToView (aka view matrix) and viewToWorld are inverses of eachother
    worldMatrix = XMMatrixInverse( NULL, CalculateViewMatrix( ) );
    return worldMatrix;
}

/*
================
ArcBallCamera::GetCameraPosition
================
*/
const XMFLOAT3 ArcBallCamera::GetCameraPosition( void ) const {
	return cameraPosition;
}