/*
    ReflecTech
    ----------
    File        :    RtKeyboardXR.h
    Author      :    Jamie Taylor
    Last Edit   :    31/10/12
    Desc        :    A Raw Input-powered implementation of the keyboard interface.
*/
#ifndef RT_KEYBOARD_XR_H
#define RT_KEYBOARD_XR_H


#include "../../PlatformIndependenceLayer/RtPlatform.h"
// the interface being implemented
#include "../HID/RtKeyboard.h"


// implementation specific button/key/control codes
static const U8 KeyboardKeyCodes[KEYBOARD_KEY_COUNT] =
{
    // esc
    VK_ESCAPE,

    // function keys
    VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12,

    // numerical keys
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,

    // alphabetic keys
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 
    0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,

    // numpad
    VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9,
    VK_DECIMAL, VK_SEPARATOR, VK_ADD, VK_SUBTRACT, VK_MULTIPLY, VK_DIVIDE,  VK_NUMLOCK,

    // arrows
    VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT,

    // 'special' cluster 1
    VK_SNAPSHOT, VK_SCROLL, VK_PAUSE,

    // 'special' cluster 2
    VK_INSERT, VK_HOME, VK_NEXT, VK_DELETE, VK_PRIOR, VK_NEXT,

    // left & right variant keys
    VK_LSHIFT, VK_RSHIFT, VK_LCONTROL, VK_RCONTROL, 0, 0, VK_LWIN, VK_RWIN,

    // cluster 3
    0, 0,
    0, 0,
    0, 0,

    // cluster 4
    0, 0,
    0, 0,

    // OEM
    0, 0, 0, 0, 0, 0, 0, 0,
};


/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
class KeyboardXR : public Keyboard
{
public:
    // ctor/dtor
    KeyboardXR();
    ~KeyboardXR();

    // startup, reset and shutdown
    bool Startup(U32 startupParams);
     // explicitly set keyboard state (key buffers etc) to 0
    void Reset();
    void Shutdown();

    // update/poll the keyboard
    void Update(U32 updateParams);
    
    bool IsUp(U32 key) const;
    bool IsDown(U32 key) const;

    // check device state
    bool IsConnected() const;

    void Enabled(bool state);
    bool IsEnabled() const;

private:
    // implementation specific members
    RAWINPUTDEVICE keyboard;

    // key states for current frame and previous frame
    U16 currentFrame;
    U16 previousFrame;
};
/**************************************************************************************************************************/


// TEMP? DECIDE ON SUBSYSTEM MEMORY ALLOCATION STYLE
extern "C" Keyboard* CreateKeyboard(void *mem);
extern "C" void DestroyKeyboard(Keyboard *p);
extern "C" U32 GetKeyboardMemReq();


#endif // RT_KEYBOARD_XR_H
