/*
	ReflecTech
	----------
	File		:	RtMouse.h
	Author		:	Jamie Taylor
	Last Edit	:	31/10/12
	Desc		:	The interface for a mouse.

					The startup & updateParams are intended to allow any implementation to pass specific information needed for polling/updating.
					The idea is to pass some integer value if that's all that's required, otherwise the implementation could define a struct containing
					information it needs to update/poll the controller; in this case the parameter could be used to pass the memory address of a struct instance.
*/
#ifndef RT_MOUSE_H
#define RT_MOUSE_H


#include "../../PlatformIndependenceLayer/RtPlatform.h"


// Intended to be used for indexing into an OS/API specific keyCode array
// REGARDLESS of exact implementation, engine/application code should be able to use these enum values and be shielded from OS/API specifics
enum RT_MOUSE
{
	// standard left, right and middle mouse buttons
	LEFT_BUTTON,
	RIGHT_BUTTON,
	MIDDLE_BUTTON,

	// 'X button(s)' - extra buttons outside the standard left, right and middle
	X1_BUTTON,
	X2_BUTTON,

	// used for array indexing
	MOUSE_BUTTON_COUNT,
};


// struct to describe x,y movement/position
struct Coordinates
{
	U32 x;
	U32 y;
};
typedef struct Coordinates Movement, Position, Cage;


// struct to hold/describe mouse state, unlike a keyboard, a mouse has position and movement to track
// along with key states, hence a small struct is made
struct MouseState
{
	U16 buttons;
	U16 wheelDelta;

	Movement movement;
	Position position;

	MouseState& operator=(MouseState &rhs);
	MouseState& operator=(U32 setTo);
};


/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
class Mouse
{
public:
	// dtor
	virtual ~Mouse() {};

	// startup, reset and shutdown
	virtual bool Startup(U32 startupParams = 0) = 0;
 	// explicitly set mouse state (key buffers, movement, wheel delta etc) to 0	
	virtual void Reset() = 0;
	virtual void Shutdown() = 0;

	// update/poll the mouse
	virtual void Update(U32 updateParams = 0) = 0;

	virtual bool IsUp(U32 button) const = 0;
	virtual bool IsDown(U32 button) const = 0;

	// get relative movement
	virtual Movement& GetMovement() const = 0;
	// get/set absolute position
	virtual void SetPosition(U32 x, U32 y) = 0;
	virtual Position& GetPosition() const = 0;
	// get wheel delta
	virtual U16 GetWheelDelta() const = 0;

	// get/set cage - a cage can restrict the mouses movement to a particular region
	virtual bool IsCaged() const = 0;
	virtual Cage& GetCage() const = 0;
	virtual void SetCage(U32 x, U32 y) = 0;

	// check device state
	virtual bool IsConnected() const = 0; // in the current implementation, since there's no way to discern between mouse & keyboard we return true always

	virtual void Enabled(bool state) = 0;
	virtual bool IsEnabled() const = 0;

protected:
	// all implementations will inherit these members and then add their own implementation specific private members
	bool isConnected;
	bool isEnabled;

	bool isCaged;
	Cage cage;

	// bool invertXaxis;
	// bool invertYaxis

	MouseState currentFrame;
	MouseState previousFrame;
};
/**************************************************************************************************************************/


#endif // RT_MOUSE_H
