/*
	ReflecTech
	----------
	File		:	RtGamepadXR.h
	Author		:	Jamie Taylor
	Last Edit	:	27/10/12
	Desc		:	An Xinput-powered implementation of the Gamepad interface.
*/  
#ifndef RT_GAMEPAD_XR_H
#define RT_GAMEPAD_XR_H


#include "../../PlatformIndependenceLayer/RtPlatform.h"
//#include "../../PlatformIndependenceLayer/RtTimer.h"

// the interface being implemented
#include "../HID/RtGamepad.h"
#include <XInput.h>

// needed to test factory functions
#include "../../CoreSystems/RtHeapAllocator.h"


// analog stick dead-zones
#define LEFT_STICK_DEADZONE	    XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
#define RIGHT_STICK_DEADZONE    XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
// trigger dead-zones
#define LEFT_TRIGGER_DEADZONE   XINPUT_GAMEPAD_TRIGGER_THRESHOLD
#define RIGHT_TRIGGER_DEADZONE  XINPUT_GAMEPAD_TRIGGER_THRESHOLD
// max trigger values
#define MAX_LEFT_TRIGGER_VALUE  255.0f
#define MAX_RIGHT_TRIGGER_VALUE 255.0f


// implementation specific button/key/control codes
static const U16 GamepadButtonCodes[GAMEPAD_BUTTON_COUNT] = 
{
	// center
	XINPUT_GAMEPAD_START,
	XINPUT_GAMEPAD_BACK,

	// left pad ("dpad", < ^ > etc)
	XINPUT_GAMEPAD_DPAD_UP,
	XINPUT_GAMEPAD_DPAD_DOWN,
	XINPUT_GAMEPAD_DPAD_LEFT,
	XINPUT_GAMEPAD_DPAD_RIGHT,

	// right pad (A,B, O,X etc)
	XINPUT_GAMEPAD_Y,
	XINPUT_GAMEPAD_A,
	XINPUT_GAMEPAD_X,
	XINPUT_GAMEPAD_B,

	// bumpers/shoulder buttons (LB, RB, L1, R1)
	XINPUT_GAMEPAD_LEFT_SHOULDER,
	XINPUT_GAMEPAD_RIGHT_SHOULDER,

	// triggers
	// left
	0, 
	// right
	0, 

	// analog sticks
	// stick
	0,
	// quadrants
	0, 0, 0, 0,
	// stick down
	XINPUT_GAMEPAD_LEFT_THUMB,

	// stick
	0,
	// quadrants
	0, 0, 0, 0,
	// stick down
	XINPUT_GAMEPAD_RIGHT_THUMB,
};


/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
class GamepadXR : public Gamepad
{
public:
	// dtor
	GamepadXR(); // ctor not present in interface
	~GamepadXR();

	// startup, reset and shutdown
	bool Startup(U32 startupParams);
	// explicitly set key buffers/GamepadXR state to 0
	void Reset(); 
	void Shutdown();

	// update the GamepadXR state
	void Update(U32 paramAddress);

	// standard digital buttons (X, Y / X, O etc...) analog stick quadrants should also be compatible with these functions
	bool IsUp(U32 button) const;
	bool IsDown(U32 button) const;
	bool IsBeingHeld(U32 button) const;

	// triggers
	I16 GetTriggerValue(U32 trigger) const;
	F32 GetNormalisedTriggerValue(U32 trigger) const;

	// analog sticks
	AnalogStick& GetStickPosition(U32 stick) const;
	NormalisedAnalogStick GetNormalisedStickPosition(U32 stick) const;

	// vibrate the controller
	void SetVibration(I16 left, I16 right, F32 duration);

	// check device state
	bool IsConnected() const;
	void Enabled(bool state);
	bool IsEnabled() const;

private:
	// declare implementation specfic private members, in addition to implementation independant protected members inherited from interface
	//Timer timer;
	XINPUT_STATE gamepad;
};
/**************************************************************************************************************************/


/**************************************************************************************************************************/
// Factory functions - uses an allocator to create an instance of the graphics device
/**************************************************************************************************************************/
template<class Allocator>
Gamepad* CreateGamepad(Allocator &allctr)
{
	GamepadXR *temp = reinterpret_cast<GamepadXR*>(allctr.Allocate(sizeof(GamepadXR)));
	allctr.Construct(temp);
	return reinterpret_cast<Gamepad*>(temp);
}

template<class Allocator>
void DestroyGamepad(Gamepad *destroyThis, Allocator &allctr)
{
	Gamepad *temp = reinterpret_cast<Gamepad*>(destroyThis);
	allctr.Destruct(temp);
	allctr.DeAllocate(temp);
}

// can't export templates, may have to hardcode these for each allocator type? boilerplate style?
extern "C" Gamepad* CreateGamepadFromHeap(HeapAllocator<Gamepad> &allctr);
extern "C" void DestroyGamepadFromHeap(Gamepad *p, HeapAllocator<Gamepad> &allctr);


// ALT CREATE & DESTROY FOR TESTING
extern "C" Gamepad* CreateGamepadAlt(void *mem);
extern "C" void DestroyGamepadAlt(Gamepad *p);
extern "C" U32 GetMemReq();
/**************************************************************************************************************************/


#endif // RT_GAMEPAD_XR_H