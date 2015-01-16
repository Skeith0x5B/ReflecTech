/*
===============================================================================

	ReflecTech
	==========
	File		:	RtRenderingUtils.h
	Author		:	Jamie Taylor
	Last Edit	:	01/07/13
	Desc		:	Common and useful utility code for rendering.

===============================================================================
*/


#ifndef RT_RENDERING_UTILS_H
#define RT_RENDERING_UTILS_H


#include "../../PlatformIndependenceLayer/RtPlatform.h"


/*
===============================================================================

Colours namespace
- We use a namespace here as an enum doesn't allow for anything more
  complex than an int

===============================================================================
*/
namespace Colours {
	const F32 White[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const F32 Black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	// primaries
	const F32 Red[4]   = { 1.0f, 0.0f, 0.0f, 1.0f };
	const F32 Green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	const F32 Blue[4]  = { 0.0f, 0.0f, 1.0f, 1.0f };

	const F32 DefaultBlue[4] = { 0.0f, 0.0f, 0.25f, 1.0f };
	const F32 DefaultGrey[4] = { 0.75f, 0.75f, 0.75f, 1.0f };
};


/*
===============================================================================

Render States

===============================================================================
*/
enum RenderStates {
	Solid = 0,
	Wirefrmae = 1,
};


#endif // RT_RENDERING_UTILS_H