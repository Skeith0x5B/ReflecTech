/*
	ReflecTech
	----------
	File		:	RtKeyboardXR.cpp
	Author		:	Jamie Taylor
	Last Edit	:	31/10/12
	Desc		:	A Raw Input-powered implementation of the keyboard U32erface.
*/
#include "RtKeyboardXR.h"


/**************************************************************************************************************************/
// Implementation 
/**************************************************************************************************************************/
KeyboardXR::KeyboardXR()
{
	currentFrame = 0;
	previousFrame = 0;

	// we always return true in this implementation as we can't discern between mouse and keyboard with WM_DEVICECHANGE
	isConnected = true;
	isEnabled = false;
}
/**************************************************************************************************************************/

KeyboardXR::~KeyboardXR()
{
	Shutdown();
}
/**************************************************************************************************************************/

bool KeyboardXR::Startup(U32 startupParams)
{
	keyboard.usUsagePage = 0x01;
	keyboard.usUsage	 = 0x06;
	keyboard.dwFlags	 = RIDEV_NOLEGACY | RIDEV_NOHOTKEYS; // do not generate messages like WM_KEYDOWN (RIDEV_NOLEGACY) | No Windows Key (RIDEV_NOHOTKEYS)
	keyboard.hwndTarget	 = reinterpret_cast<HWND>(startupParams);

	isEnabled = true;

	return RegisterRawInputDevices(&keyboard, 1, sizeof(RAWINPUTDEVICE));
}
/**************************************************************************************************************************/
 
void KeyboardXR::Reset()
{
	currentFrame = 0;
	previousFrame = 0;
}
/**************************************************************************************************************************/

void KeyboardXR::Shutdown()
{
	currentFrame = 0;
	previousFrame = 0;

	isEnabled = false;
	isConnected = false;
}
/**************************************************************************************************************************/

void KeyboardXR::Update(U32 updateParams)
{
	// update buffers - update here so when the keyboard is disabled we don't get false positives
	previousFrame = currentFrame;
	currentFrame = 0;


	// don't update the device if it's not enabled/setup
	if(!isEnabled)
		return;		


	// get input using RawInput
	I8 buffer[sizeof(RAWINPUT)] = {};
	U32 size = sizeof(RAWINPUT);
	U32 readKeyboard = GetRawInputData(reinterpret_cast<HRAWINPUT>(updateParams), RID_INPUT, &buffer, &size, sizeof(RAWINPUTHEADER));

	RAWINPUT *raw = reinterpret_cast<RAWINPUT*>(buffer);
	RAWKEYBOARD &keyboard = raw->data.keyboard;

	// sometimes a 'fake key' will be sent as part of an esacape sequence 
	// - identified by virtual key code 255 - don't bother with these
	if(keyboard.VKey == 255)
		return;

	U32 vKeyCode = static_cast<U32>(keyboard.VKey); 
	U32 scanCode = static_cast<U32>(keyboard.MakeCode);
	U32 rawFlags = static_cast<U32>(keyboard.Flags);
	bool isE0 = (rawFlags & RI_KEY_E0) != 0;
	bool isE1 = (rawFlags & RI_KEY_E1 )!= 0;

	// Handle special cases
	// handle shift, control and alt
	if (vKeyCode == VK_SHIFT || vKeyCode == VK_CONTROL || vKeyCode == VK_MENU)
	{
		// shift doesn't set E0 correctly anyway - hence the call to Map() gives us the correct value and adding E0 (0) 
		// is simply null&void -> however E0 is added for ctrl & menu to decipher left from right as the call to Map() isn't enough in these cases
		vKeyCode = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX) + static_cast<U32>(isE0);
	}

	// NUMLOCK returns a different virtual key code but the same scancode as PAUSE - correct that
	if(vKeyCode == VK_NUMLOCK)
	{
		scanCode = (MapVirtualKey(vKeyCode, MAPVK_VK_TO_VSC) | 0x100);
	}

	// HIJACK THIS AND HANDLE ESCAPE SEQUENCES? - E1 escape sequences?
	if(vKeyCode == VK_PAUSE)
	{
		// MapVirtualKey() can't map VK_PAUSE due to an API bug
		scanCode = 0x45;
	}

	// num pad numbers should always be read as numbers
	switch(vKeyCode)
	{
		case VK_INSERT:
			if(!isE0)
				vKeyCode = RT_KEYBOARD::NUMPAD_0;
			break;

		case VK_DELETE:
			if(!isE0)
				vKeyCode = RT_KEYBOARD::NUMPAD_DECIMAL;
			break;

		// num pad enter has it's e0 flag set - typical enter key does not
		case VK_RETURN:
			if(isE0)
				vKeyCode = RT_KEYBOARD::NUMPAD_ENTER;
			break;

		case VK_END:
			if(!isE0)
				vKeyCode = RT_KEYBOARD::NUMPAD_1;
			break;

		case VK_DOWN:
			if(!isE0)
				vKeyCode = RT_KEYBOARD::NUMPAD_2;
			break;

		case VK_PRIOR:
			if(!isE0)
				vKeyCode = RT_KEYBOARD::NUMPAD_3;
			break;

		case VK_LEFT:
			if(!isE0)
				vKeyCode = RT_KEYBOARD::NUMPAD_4;
			break;

		case VK_CLEAR:
			if(!isE0)
				vKeyCode = RT_KEYBOARD::NUMPAD_5;
			break;

		case VK_RIGHT:
			if(!isE0)
				vKeyCode = RT_KEYBOARD::NUMPAD_6;
			break;

		case VK_HOME:
			if(!isE0)
				vKeyCode = RT_KEYBOARD::NUMPAD_7;
			break;

		case VK_UP:
			if(!isE0)
				vKeyCode = RT_KEYBOARD::NUMPAD_8;
			break;

		case VK_NEXT:
			if(!isE0)
				vKeyCode = RT_KEYBOARD::NUMPAD_9;
			break;
	}

	// update frame key state buffers
	//currentFrame |= vKeyCode;
	if(!(keyboard.Flags & RI_KEY_BREAK))
		currentFrame = vKeyCode;
}
/**************************************************************************************************************************/
	
bool KeyboardXR::IsUp(U32 key) const
{
	return ((previousFrame == KeyboardKeyCodes[key]) && ((currentFrame != KeyboardKeyCodes[key]))) != 0;
}
/**************************************************************************************************************************/

bool KeyboardXR::IsDown(U32 key) const
{
	return (currentFrame == KeyboardKeyCodes[key]) != 0;
}
/**************************************************************************************************************************/

bool KeyboardXR::IsConnected() const
{
	return isConnected;
}
/**************************************************************************************************************************/

void KeyboardXR::Enabled(bool state)
{
	isEnabled = state;
}
/**************************************************************************************************************************/

bool KeyboardXR::IsEnabled() const
{
	return isEnabled;
}
/**************************************************************************************************************************/


// ALT CREATE & DESTROY FOR TESTING

//#include <new>
#include "../../CoreSystems/RtHeapAllocator.h" // needs a definition of placement new - TODO: FIX THIS/DECIDE ON SUBSYSTEM MEMORY ALLOCATION SYTLE

Keyboard* CreateKeyboard(void *mem)
{
	KeyboardXR *temp = new(mem)KeyboardXR;
	return reinterpret_cast<Keyboard*>(temp);
}

void DestroyKeyboard(Keyboard *p)
{
	// destroy
	//delete p;
	reinterpret_cast<KeyboardXR*>(p)->~KeyboardXR();
}

U32 GetKeyboardMemReq()
{
	return sizeof(KeyboardXR);
}