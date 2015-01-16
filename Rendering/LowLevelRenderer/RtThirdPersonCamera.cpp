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


#include "RtThirdPersonCamera.h"


/*
================
ThirdPersonCamera::ThirdPersonCamera
================
*/
ThirdPersonCamera::ThirdPersonCamera( void ) {
    cameraPosition = XMFLOAT3( 0.0f, 0.0f, 0.0f );
    cameraTarget = XMFLOAT3( 0.0f, 0.0f, 1.0f );

    xBasisVector.x = 1.0f; xBasisVector.y = 0.0f; xBasisVector.z = 0.0f;
    yBasisVector.x = 0.0f; yBasisVector.y = 1.0f; yBasisVector.z = 0.0f;
    zBasisVector.x = 0.0f; zBasisVector.y = 0.0f; zBasisVector.z = 1.0f;

	pitch = yaw = 0.0f;
	infinitePitch = infiniteYaw = true;

	zoom = 0.0f;

	worldToView = XMMatrixIdentity();
	viewToWorld = XMMatrixIdentity();
}

/*
================
ThirdPersonCamera::ThirdPersonCamera
================
*/
ThirdPersonCamera::ThirdPersonCamera( const XMFLOAT3 *target ) {
	cameraPosition = XMFLOAT3( 0.0f, 0.0f, 0.0f );
    cameraTarget = *target;

    xBasisVector.x = 1.0f; xBasisVector.y = 0.0f; xBasisVector.z = 0.0f;
    yBasisVector.x = 0.0f; yBasisVector.y = 1.0f; yBasisVector.z = 0.0f;
    zBasisVector.x = 0.0f; zBasisVector.y = 0.0f; zBasisVector.z = 1.0f;

	pitch = yaw = 0.0f;
	infinitePitch = infiniteYaw = true;

	zoom = 0.0f;

	worldToView = XMMatrixIdentity();
	viewToWorld = XMMatrixIdentity();
}

/*
================
ThirdPersonCamera::SetPosition
================
*/
void ThirdPersonCamera::SetPosition( const XMFLOAT3 *position ) {
	cameraPosition = *position;
}

/*
================
ThirdPersonCamera::SetTarget
================
*/
void ThirdPersonCamera::SetTarget( const XMFLOAT3 *target ) {
	cameraTarget = *target;
}

/*
================
ThirdPersonCamera::SetZoomLimits
================
*/
void ThirdPersonCamera::SetZoom( F32 _minZoom, F32 currentZoom, F32 _maxZoom ) {
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
ThirdPersonCamera::Zoom
================
*/
void ThirdPersonCamera::Zoom( F32 zoomDelta ) {
	zoom += zoomDelta;

	if( zoom > maxZoom ) {
		zoom = maxZoom;
	} else if( zoom < minZoom ) {
		zoom = minZoom;
	}
}

/*
================
ThirdPersonCamera::SetRotation
================
*/
void ThirdPersonCamera::SetRotation( F32 _minPitch, F32 currentPitch, F32 _maxPitch, bool _infinitePitch, 
								 F32 _minYaw, F32 currentYaw, F32 _maxYaw, bool _infiniteYaw) {
	minPitch = _minPitch;
	pitch = currentPitch;
	maxPitch = _maxPitch;

	minYaw = _minYaw;
	yaw = currentYaw;
	maxYaw = _maxYaw;

	infinitePitch = _infinitePitch;
	infiniteYaw = _infiniteYaw;

	if( ( pitch > maxPitch ) && ( infinitePitch == false ) ) {
		pitch = maxPitch;
	} else if( ( pitch < minPitch ) && ( infinitePitch == false ) ) {
		pitch = minPitch;
	}

	if( ( pitch > maxYaw ) && ( infiniteYaw == false ) ) {
		yaw = maxYaw;
	} else if( ( pitch < minYaw ) && ( infiniteYaw == false ) ) {
		yaw = minYaw;
	}
}

/*
================
ThirdPersonCamera::Rotate
================
*/
void ThirdPersonCamera::Rotate( F32 pitchDelta, F32 yawDelta ) {
	pitch += pitchDelta;
	yaw	+= yawDelta;

	if( ( pitch > maxPitch ) && ( infinitePitch == false ) ) {
		pitch = maxPitch;
	} else if( ( pitch < minPitch ) && ( infinitePitch == false ) ) {
		pitch = minPitch;
	}

	if( ( pitch > maxYaw ) && ( infiniteYaw == false ) ) {
		yaw = maxYaw;
	} else if( ( pitch < minYaw ) && ( infiniteYaw == false ) ) {
		yaw = minYaw;
	}
}

/*
================
ThirdPersonCamera::Translate
----------------
Bit of vector addition needed here to ensure we move along an arbitrary axis of our choosing.
We take a reference vector along Z (0, 0, distance, 1), rotate it by yaw and add it to
out position.
================
*/
void ThirdPersonCamera::Translate( F32 xDelta, F32 yDelta, F32 zDelta ) {
	//XMVECTOR zRef = XMVectorSet( xDelta, 0.0f, zDelta, 1.0f );
	//XMMATRIX rota = XMMatrixRotationY( yaw );
	//zRef = XMVector3Transform( zRef, rota );
	//
	//XMFLOAT3 zRefTransformed;
	//XMStoreFloat3( &zRefTransformed, zRef );

	//cameraTarget.x += zRefTransformed.x;
	//cameraTarget.z += zRefTransformed.z;

	//cameraPosition.x += zRefTransformed.x;
	//cameraPosition.z += zRefTransformed.z;
}

/*
================
ThirdPersonCamera::CalculateWorldToViewMatrix
================
*/
const XMMATRIX ThirdPersonCamera::CalculateWorldToViewMatrix( void ) {
	// create view matrix
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw( pitch, yaw, 0.0f );
	XMVECTOR zReference = XMVectorSet( 0.0f, 0.0f, zoom, 1.0f );
	XMVECTOR newUp = XMLoadFloat3( &yBasisVector );

	zReference = XMVector3Transform( zReference, rotation );
	newUp = XMVector3Transform( newUp, rotation );

	XMVECTOR position = XMLoadFloat3( &cameraPosition );
	XMVECTOR target = XMLoadFloat3( &cameraTarget );
	position = target + zReference; // zReference + target;
	XMStoreFloat3( &cameraPosition, position );

	worldToView = XMMatrixLookAtLH( position, target, newUp  );
  	return worldToView;
}

/*
================
ThirdPersonCamera::CalculateViewToWorldMatrix
================
*/
const XMMATRIX ThirdPersonCamera::CalculateViewToWorldMatrix( void ) {
	// worldToView (aka view matrix) and viewToWorld are inverses of eachother
    viewToWorld = XMMatrixInverse( NULL, CalculateWorldToViewMatrix( ) );
    return viewToWorld;
}
