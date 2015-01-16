/*
===============================================================================

	ReflecTech
	==========
	File		:	RtFirstPersonCamera.h, DSV = Doom Syntax Version
	Author		:	Jamie Taylor
	Last Edit	:	28/01/13
	Desc		:	A basic first-person camera class.

                    TODO: - Add camera frustum for possible frustum culling?

===============================================================================
*/


#include "RtFirstPersonCamera.h"


/*
================
FirstPersonCamera::FirstPersonCamera
================
*/
FirstPersonCamera::FirstPersonCamera( void ) {
    cameraPosition = XMFLOAT3( 0.0f, 0.0f, 0.0f );
    cameraTarget = XMFLOAT3( 0.0f, 0.0f, 1.0f );

	pitch = yaw = 0.0f;
	infinitePitch = infiniteYaw = true;

	worldToView = XMMatrixIdentity();
	viewToWorld = XMMatrixIdentity();
}

/*
================
FirstPersonCamera::SetPosition
================
*/
void FirstPersonCamera::SetPosition( const XMFLOAT3 *position ) {
	cameraPosition = *position;
}

/*
================
FirstPersonCamera::SetTarget
================
*/
void FirstPersonCamera::SetTarget( const XMFLOAT3 *target ) {
	cameraTarget = *target;
}

/*
================
FirstPersonCamera::SetRotation
================
*/
void FirstPersonCamera::SetRotation( F32 _minPitch, F32 currentPitch, F32 _maxPitch, bool _infinitePitch, 
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
FirstPersonCamera::Rotate
================
*/
void FirstPersonCamera::Rotate( F32 pitchDelta, F32 yawDelta ) {
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
FirstPersonCamera::Translate
----------------
Bit of vector addition needed here to ensure we move along an arbitrary axis of our choosing.
We take a reference vector along Z (0, 0, distance, 1), rotate it by yaw and add it to
out position.
================
*/
void FirstPersonCamera::Translate( F32 xDelta, F32 yDelta, F32 zDelta ) {
	XMVECTOR zRef = XMVectorSet( xDelta, 0.0f, zDelta, 1.0f ); // xDelta not needed here
	XMMATRIX rota = XMMatrixRotationY( yaw );
	zRef = XMVector3Transform( zRef, rota );
	
	XMFLOAT3 zRefTransformed;
	XMStoreFloat3( &zRefTransformed, zRef );

	cameraPosition.x += zRefTransformed.x;
	cameraPosition.z += zRefTransformed.z;
}

/*
================
FirstPersonCamera::CalculateWorldToViewMatrix
================
*/
const XMMATRIX FirstPersonCamera::CalculateWorldToViewMatrix( void ) {
	// create view matrix
	XMVECTOR zReference = XMVectorSet( 0.0f, 0.0f, 1.0f, 1.0f );
	XMVECTOR up = XMVectorSet( 0.0f, 1.0f, 0.0f, 1.0f );
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw( pitch, yaw, 0.0f );

	XMVECTOR target = XMLoadFloat3( &cameraTarget );
	XMVECTOR position = XMLoadFloat3( &cameraPosition );

	zReference = XMVector3Transform( zReference, rotation );
	up = XMVector3Transform( up, rotation );

	target = zReference + position;
	XMStoreFloat3( &cameraTarget, target );

	worldToView = XMMatrixLookAtLH( position, target, up );
  	return worldToView;
}

/*
================
FirstPersonCamera::CalculateViewToWorldMatrix
================
*/
const XMMATRIX FirstPersonCamera::CalculateViewToWorldMatrix( void ) {
	// worldToView (aka view matrix) and viewToWorld are inverses of eachother
    viewToWorld = XMMatrixInverse( NULL, CalculateWorldToViewMatrix( ) );
    return viewToWorld;
}
