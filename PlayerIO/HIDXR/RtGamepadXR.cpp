/*
	ReflecTech
	----------
	File		:	RtGamepadXR.h
	Author		:	Jamie Taylor
	Last Edit	:	27/10/12
	Desc		:	An Xinput-powered implementation of the Gamepad interface.
*/  
#include "RtGamepadXR.h"


/**************************************************************************************************************************/
// Implementation 
/**************************************************************************************************************************/
GamepadXR::GamepadXR()
{
	// check we're connected
	ZeroMemory(&gamepad, sizeof(XINPUT_STATE));
	isConnected = ((XInputGetState(0, &gamepad)) != ERROR_DEVICE_NOT_CONNECTED);

	isEnabled = false;
}
/**************************************************************************************************************************/

GamepadXR::~GamepadXR()
{
	Shutdown();
}
/**************************************************************************************************************************/

bool GamepadXR::Startup(U32 startupParams)
{
	currentFrame = 0;
	previousFrame = 0;

	vibrationStartTime = 0.0f;
	vibrationDuration = 0.0f;

	isEnabled = true;

	return true;
}
/**************************************************************************************************************************/

void GamepadXR::Reset()
{
	currentFrame = 0;
	previousFrame = 0;

	vibrationStartTime = 0.0f;
	vibrationDuration = 0.0f;
}
/**************************************************************************************************************************/

void GamepadXR::Shutdown()
{
	isEnabled = false;
}
/**************************************************************************************************************************/

void GamepadXR::Update(U32 paramAddress)
{
	// update previous & current frame states
	previousFrame = currentFrame;
	currentFrame = 0;

	// is vibration active? if so, has it timed out?
	//if(vibrationDuration > 0)
		//if(timer.GetMillisecondsF() - vibrationStartTime >= vibrationDuration)
			//SetVibration(0, 0, 0.0f);
	

	// don't update the device if it's not enabled/setup
	if(!isEnabled)
		return;	


	ZeroMemory(&gamepad, sizeof(XINPUT_STATE));

	// only update the controller if it's connected
	U32 result = XInputGetState(0, &gamepad);
	if(result == ERROR_SUCCESS)
	{
		// set controller state
		currentFrame.buttons = gamepad.Gamepad.wButtons;

		// update triggers using deadzones
		if(gamepad.Gamepad.bLeftTrigger > LEFT_TRIGGER_DEADZONE)
			currentFrame.leftTrigger = gamepad.Gamepad.bLeftTrigger;		
		if(gamepad.Gamepad.bRightTrigger > RIGHT_TRIGGER_DEADZONE)
			currentFrame.rightTrigger = gamepad.Gamepad.bRightTrigger;

		// update analog sticks using deadzones
		if((gamepad.Gamepad.sThumbLX > LEFT_STICK_DEADZONE) || (gamepad.Gamepad.sThumbLX < (-LEFT_STICK_DEADZONE))) 
			currentFrame.leftStick.x = gamepad.Gamepad.sThumbLX;
		if((gamepad.Gamepad.sThumbLY > LEFT_STICK_DEADZONE) || (gamepad.Gamepad.sThumbLY < (-LEFT_STICK_DEADZONE))) 
			currentFrame.leftStick.y = gamepad.Gamepad.sThumbLY;

		if((gamepad.Gamepad.sThumbRX > RIGHT_STICK_DEADZONE) || (gamepad.Gamepad.sThumbRX < (-RIGHT_STICK_DEADZONE))) 
			currentFrame.rightStick.x = gamepad.Gamepad.sThumbRX;
		if((gamepad.Gamepad.sThumbRY > RIGHT_STICK_DEADZONE) || (gamepad.Gamepad.sThumbRY < (-RIGHT_STICK_DEADZONE))) 
			currentFrame.rightStick.y = gamepad.Gamepad.sThumbRY;

		// controller is connected
		isConnected = true;
	}
	else if(result == ERROR_DEVICE_NOT_CONNECTED)
	{
		// controller isn't connected - don't/can't update
		isConnected = false;
	}
}
/**************************************************************************************************************************/

bool GamepadXR::IsUp(U32 button) const
{
	// check triggers
	if(button == RT_GAMEPAD::LEFT_TRIGGER)
		return ((previousFrame.leftTrigger) != 0) && ((currentFrame.leftTrigger) == 0);

	if(button == RT_GAMEPAD::RIGHT_TRIGGER)
		return ((previousFrame.rightTrigger) != 0) && ((currentFrame.rightTrigger) == 0);

	// stick quadrants
	// LEFT
	if(button == RT_GAMEPAD::LEFT_STICK_UPPER_LEFT_QUADRANT)
		return ((previousFrame.leftStick.x < 0) && (previousFrame.leftStick.y > 0)) != 0 && ((currentFrame.leftStick.x < 0) && (currentFrame.leftStick.y > 0)) == 0;

	if(button == RT_GAMEPAD::LEFT_STICK_UPPER_RIGHT_QUADRANT)
		return ((previousFrame.leftStick.x > 0) && (previousFrame.leftStick.y > 0)) != 0 && ((currentFrame.leftStick.x > 0) && (currentFrame.leftStick.y > 0)) == 0;

	if(button == RT_GAMEPAD::LEFT_STICK_LOWER_LEFT_QUADRANT)
		return ((previousFrame.leftStick.x < 0) && (previousFrame.leftStick.y < 0)) != 0 && ((currentFrame.leftStick.x < 0) && (currentFrame.leftStick.y < 0)) == 0;

	if(button == RT_GAMEPAD::LEFT_STICK_LOWER_RIGHT_QUADRANT)
		return ((previousFrame.leftStick.x > 0) && (previousFrame.leftStick.y < 0)) != 0 && ((currentFrame.leftStick.x > 0) && (currentFrame.leftStick.y < 0)) == 0;

	// RIGHT
	if(button == RT_GAMEPAD::RIGHT_STICK_UPPER_LEFT_QUADRANT)
		return ((previousFrame.rightStick.x < 0) && (previousFrame.rightStick.y > 0)) != 0 && ((currentFrame.rightStick.x < 0) && (currentFrame.rightStick.y > 0)) == 0;

	if(button == RT_GAMEPAD::RIGHT_STICK_UPPER_RIGHT_QUADRANT)
		return ((previousFrame.rightStick.x > 0) && (previousFrame.rightStick.y > 0)) != 0 && ((currentFrame.rightStick.x > 0) && (currentFrame.rightStick.y > 0)) == 0;

	if(button == RT_GAMEPAD::RIGHT_STICK_LOWER_LEFT_QUADRANT)
		return ((previousFrame.rightStick.x < 0) && (previousFrame.rightStick.y < 0)) != 0  && ((currentFrame.rightStick.x < 0) && (currentFrame.rightStick.y < 0)) == 0;

	if(button == RT_GAMEPAD::RIGHT_STICK_LOWER_RIGHT_QUADRANT)
		return ((previousFrame.rightStick.x > 0) && (previousFrame.rightStick.y < 0)) != 0 && ((currentFrame.rightStick.x > 0) && (currentFrame.rightStick.y < 0)) == 0;
				
	// digital buttons
	return (previousFrame.buttons & GamepadButtonCodes[button]) != 0 && (currentFrame.buttons & GamepadButtonCodes[button]) == 0;
}
/**************************************************************************************************************************/

bool GamepadXR::IsDown(U32 button) const
{
	// check triggers
	if(button == RT_GAMEPAD::LEFT_TRIGGER)
		return ((currentFrame.leftTrigger) != 0) && ((previousFrame.leftTrigger) == 0);

	if(button == RT_GAMEPAD::RIGHT_TRIGGER)
		return ((currentFrame.rightTrigger) != 0) && ((previousFrame.rightTrigger) == 0);

	// stick quadrants
	// stick quadrants
	// LEFT
	if(button == RT_GAMEPAD::LEFT_STICK_UPPER_LEFT_QUADRANT)
		return ((currentFrame.leftStick.x < 0) && (currentFrame.leftStick.y > 0)) != 0 && ((previousFrame.leftStick.x < 0) && (previousFrame.leftStick.y > 0)) == 0;

	if(button == RT_GAMEPAD::LEFT_STICK_UPPER_RIGHT_QUADRANT)
		return ((currentFrame.leftStick.x > 0) && (currentFrame.leftStick.y > 0)) != 0 && ((previousFrame.leftStick.x > 0) && (previousFrame.leftStick.y > 0)) == 0;

	if(button == RT_GAMEPAD::LEFT_STICK_LOWER_LEFT_QUADRANT)
		return ((currentFrame.leftStick.x < 0) && (currentFrame.leftStick.y < 0)) != 0 && ((previousFrame.leftStick.x < 0) && (previousFrame.leftStick.y < 0)) == 0;

	if(button == RT_GAMEPAD::LEFT_STICK_LOWER_RIGHT_QUADRANT)
		return ((currentFrame.leftStick.x > 0) && (currentFrame.leftStick.y < 0)) != 0 && ((previousFrame.leftStick.x > 0) && (previousFrame.leftStick.y < 0)) == 0;

	// RIGHT
	if(button == RT_GAMEPAD::RIGHT_STICK_UPPER_LEFT_QUADRANT)
		return ((currentFrame.rightStick.x < 0) && (currentFrame.rightStick.y > 0)) != 0 && ((previousFrame.rightStick.x < 0) && (previousFrame.rightStick.y > 0)) == 0;

	if(button == RT_GAMEPAD::RIGHT_STICK_UPPER_RIGHT_QUADRANT)
		return ((currentFrame.rightStick.x > 0) && (currentFrame.rightStick.y > 0)) != 0 && ((previousFrame.rightStick.x > 0) && (previousFrame.rightStick.y > 0)) == 0;

	if(button == RT_GAMEPAD::RIGHT_STICK_LOWER_LEFT_QUADRANT)
		return ((currentFrame.rightStick.x < 0) && (currentFrame.rightStick.y < 0)) != 0  && ((previousFrame.rightStick.x < 0) && (previousFrame.rightStick.y < 0)) == 0;

	if(button == RT_GAMEPAD::RIGHT_STICK_LOWER_RIGHT_QUADRANT)
		return ((currentFrame.rightStick.x > 0) && (currentFrame.rightStick.y < 0)) != 0 && ((previousFrame.rightStick.x > 0) && (previousFrame.rightStick.y < 0)) == 0;
				
	// digital buttons
	//return (currentFrame.buttons & GamepadButtonCodes[button]) != 0;
	return (currentFrame.buttons & GamepadButtonCodes[button]) != 0 && (previousFrame.buttons & GamepadButtonCodes[button]) == 0;
}
/**************************************************************************************************************************/

bool GamepadXR::IsBeingHeld(U32 button) const {
	// check triggers
	if(button == RT_GAMEPAD::LEFT_TRIGGER)
		return ((currentFrame.leftTrigger) != 0) && ((previousFrame.leftTrigger) != 0);

	if(button == RT_GAMEPAD::RIGHT_TRIGGER)
		return ((currentFrame.rightTrigger) != 0) && ((previousFrame.rightTrigger) != 0);

	// stick quadrants
	// stick quadrants
	// LEFT
	if(button == RT_GAMEPAD::LEFT_STICK_UPPER_LEFT_QUADRANT)
		return ((currentFrame.leftStick.x < 0) && (currentFrame.leftStick.y > 0)) != 0 && ((previousFrame.leftStick.x < 0) && (previousFrame.leftStick.y > 0)) != 0;

	if(button == RT_GAMEPAD::LEFT_STICK_UPPER_RIGHT_QUADRANT)
		return ((currentFrame.leftStick.x > 0) && (currentFrame.leftStick.y > 0)) != 0 && ((previousFrame.leftStick.x > 0) && (previousFrame.leftStick.y > 0)) != 0;

	if(button == RT_GAMEPAD::LEFT_STICK_LOWER_LEFT_QUADRANT)
		return ((currentFrame.leftStick.x < 0) && (currentFrame.leftStick.y < 0)) != 0 && ((previousFrame.leftStick.x < 0) && (previousFrame.leftStick.y < 0)) != 0;

	if(button == RT_GAMEPAD::LEFT_STICK_LOWER_RIGHT_QUADRANT)
		return ((currentFrame.leftStick.x > 0) && (currentFrame.leftStick.y < 0)) != 0 && ((previousFrame.leftStick.x > 0) && (previousFrame.leftStick.y < 0)) != 0;

	// RIGHT
	if(button == RT_GAMEPAD::RIGHT_STICK_UPPER_LEFT_QUADRANT)
		return ((currentFrame.rightStick.x < 0) && (currentFrame.rightStick.y > 0)) != 0 && ((previousFrame.rightStick.x < 0) && (previousFrame.rightStick.y > 0)) != 0;

	if(button == RT_GAMEPAD::RIGHT_STICK_UPPER_RIGHT_QUADRANT)
		return ((currentFrame.rightStick.x > 0) && (currentFrame.rightStick.y > 0)) != 0 && ((previousFrame.rightStick.x > 0) && (previousFrame.rightStick.y > 0)) != 0;

	if(button == RT_GAMEPAD::RIGHT_STICK_LOWER_LEFT_QUADRANT)
		return ((currentFrame.rightStick.x < 0) && (currentFrame.rightStick.y < 0)) != 0  && ((previousFrame.rightStick.x < 0) && (previousFrame.rightStick.y < 0)) != 0;

	if(button == RT_GAMEPAD::RIGHT_STICK_LOWER_RIGHT_QUADRANT)
		return ((currentFrame.rightStick.x > 0) && (currentFrame.rightStick.y < 0)) != 0 && ((previousFrame.rightStick.x > 0) && (previousFrame.rightStick.y < 0)) != 0;
				
	// digital buttons
	//return (currentFrame.buttons & GamepadButtonCodes[button]) != 0;
	return (currentFrame.buttons & GamepadButtonCodes[button]) != 0 && (previousFrame.buttons & GamepadButtonCodes[button]) != 0;
}
/**************************************************************************************************************************/

I16 GamepadXR::GetTriggerValue(U32 trigger) const
{
	if(trigger == RT_GAMEPAD::LEFT_TRIGGER) 
		return currentFrame.leftTrigger;
	else if(trigger == RT_GAMEPAD::RIGHT_TRIGGER) 
		return currentFrame.rightTrigger;
}
/**************************************************************************************************************************/

F32 GamepadXR::GetNormalisedTriggerValue(U32 trigger) const
{
	if(trigger == RT_GAMEPAD::LEFT_TRIGGER)
		return (currentFrame.leftTrigger / MAX_LEFT_TRIGGER_VALUE);
	else if(trigger == RT_GAMEPAD::RIGHT_TRIGGER)
		return (currentFrame.rightTrigger / MAX_RIGHT_TRIGGER_VALUE);
}
/**************************************************************************************************************************/

AnalogStick& GamepadXR::GetStickPosition(U32 stick) const
{
	if(stick == RT_GAMEPAD::LEFT_STICK)
		return const_cast<AnalogStick&>(currentFrame.leftStick);
	else if(stick == RT_GAMEPAD::RIGHT_STICK)
		return const_cast<AnalogStick&>(currentFrame.rightStick);
}
/**************************************************************************************************************************/

NormalisedAnalogStick GamepadXR::GetNormalisedStickPosition(U32 stick) const
{
	NormalisedAnalogStick temp = {0};

	if((stick == RT_GAMEPAD::LEFT_STICK) && ((currentFrame.leftStick.x != 0) || (currentFrame.leftStick.y != 0)))
	{
		F32 magnitude = sqrt(static_cast<F32>((currentFrame.leftStick.x * currentFrame.leftStick.x)) + static_cast<F32>((currentFrame.leftStick.y * currentFrame.leftStick.y)));
		temp.x = currentFrame.leftStick.x / magnitude;
		temp.y = currentFrame.leftStick.y / magnitude;
	}
	else if(stick == RT_GAMEPAD::RIGHT_STICK && ((currentFrame.rightStick.x != 0) || (currentFrame.rightStick.y != 0)))
	{
		F32 magnitude = sqrt(static_cast<F32>((currentFrame.rightStick.x * currentFrame.rightStick.x)) + static_cast<F32>((currentFrame.rightStick.y * currentFrame.rightStick.y)));
		temp.x = currentFrame.rightStick.x / magnitude;
		temp.y = currentFrame.rightStick.y / magnitude;
	}
	
	return temp;
}
/**************************************************************************************************************************/

void GamepadXR::SetVibration(I16 left, I16 right, F32 duration)
{
	XINPUT_VIBRATION vibration = {0};
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	
	// configure
	vibration.wLeftMotorSpeed = left;
	vibration.wRightMotorSpeed = right;
	
	// set
	XInputSetState(0, &vibration);

	//vibrationStartTime = timer.GetMillisecondsF();
	vibrationDuration = duration;
}
/**************************************************************************************************************************/

bool GamepadXR::IsConnected() const
{
	return isConnected;
}
/**************************************************************************************************************************/

void GamepadXR::Enabled(bool state)
{
	isEnabled = state;
}
/**************************************************************************************************************************/

bool GamepadXR::IsEnabled() const
{
	return isEnabled;
}
/**************************************************************************************************************************/

Gamepad* CreateGamepadFromHeap(HeapAllocator<Gamepad> &allctr)
{
	return CreateGamepad(allctr);
}
/**************************************************************************************************************************/

void DestroyGamepadFromHeap(Gamepad *destroyThis, HeapAllocator<Gamepad> &allctr)
{
	DestroyGamepad(destroyThis, allctr);
}
/**************************************************************************************************************************/

// ALT CREATE & DESTROY FOR TESTING
Gamepad* CreateGamepadAlt(void *mem)
{
	GamepadXR *temp = new(mem)GamepadXR;
	return reinterpret_cast<Gamepad*>(temp);
}

void DestroyGamepadAlt(Gamepad *p)
{
	// destroy
	//delete p;
	reinterpret_cast<GamepadXR*>(p)->~GamepadXR();
}

U32 GetMemReq()
{
	return sizeof(GamepadXR);
}