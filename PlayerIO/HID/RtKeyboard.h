/*
	ReflecTech
	----------
	File		:	RtKeyboard.h
	Author		:	Jamie Taylor
	Last Edit	:	31/10/12
	Desc		:	The interface for a keyboard.

					The startup & updateParams are intended to allow any implementation to pass specific information needed for polling/updating.
					The idea is to pass some integer value if that's all that's required, otherwise the implementation could define a struct containing
					information it needs to update/poll the controller; in this case the parameter could be used to pass the memory address of a struct instance.
*/
#ifndef RT_KEYBOARD_H
#define RT_KEYBOARD_H


#include "../../PlatformIndependenceLayer/RtPlatform.h"


// Intended to be used for indexing into an OS/API specific keyCode array
// REGARDLESS of exact implementation, engine/application code should be able to use these enum values and be shielded from OS/API specifics
enum RT_KEYBOARD
{
	// esc
	ESCAPE = 0,

	// function keys
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

	// numerical keys
	NUMBER_0, NUMBER_1, NUMBER_2, NUMBER_3, NUMBER_4, NUMBER_5, NUMBER_6, NUMBER_7, NUMBER_8, NUMBER_9,

	// alphabetic keys
	A, B, C, D, E, F, G, H, I, J, K, L, M, 
	N, O, P, Q, R, S, T, U ,V, W, X, Y, Z,

	// numpad
	NUMPAD_0, NUMPAD_1, NUMPAD_2, NUMPAD_3, NUMPAD_4, NUMPAD_5, NUMPAD_6, NUMPAD_7, NUMPAD_8, NUMPAD_9, 
	NUMPAD_DECIMAL, NUMPAD_ENTER, NUMPAD_PLUS, NUMPAD_MINUS, NUMPAD_MULTIPLY, NUMPAD_DIVIDE, NUM_LOCK,

	// arrows
	ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT,

	// cluster 1
	PRINT_SCREEN, SCROLL_LOCK, PAUSE_BREAK,

	// cluster 2
	INSERT, HOME, DEL, END, PAGE_UP, PAGE_DOWN,

	// left & right variant keys
	LEFT_SHIFT, RIGHT_SHIFT, LEFT_CONTROL, RIGHT_CONTROL, LEFT_ALT, RIGHT_ALT, LEFT_WINDOWS, RIGHT_WINDOWS,

	// cluster 3
	TAB, CAPS_LOCK,
	BACK_SPACE, ENTER,
	SPACE_BAR, MENU,

	// cluster 4
	PLUS, MINUS,
	COMMA, PERIOD,

	// OEM keys - keyboard specific
	// ;:,    /?,    '@,    [{,    \|,    ]},    #~,   `¬Š (uk keyboard)
	OEM_1, OEM_2, OEM_3, OEM_4, OEM_5, OEM_6, OEM_7, OEM_8,

	KEYBOARD_KEY_COUNT,
};


/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
class Keyboard
{
public:
	// dtor
	virtual ~Keyboard() {};

	// startup, reset and shutdown
	virtual bool Startup(U32 startupParams = 0) = 0;
 	// explicitly set keyboard state (key buffers etc) to 0
	virtual void Reset() = 0;
	virtual void Shutdown() = 0;

	// update/poll the keyboard
	virtual void Update(U32 updateParams = 0) = 0;
	
	virtual bool IsUp(U32 key) const = 0;
	virtual bool IsDown(U32 key) const = 0;

	// check device state
	virtual bool IsConnected() const = 0; // in the current implementation, since there's no way to discern between mouse & keyboard we return true always

	virtual void Enabled(bool state) = 0;
	virtual bool IsEnabled() const = 0;

protected:
	// all implementations will inherit these members and then add their own implementation specific private members
	bool isConnected;
	bool isEnabled;
};
/**************************************************************************************************************************/


#endif // RT_KEYBOARD_H
