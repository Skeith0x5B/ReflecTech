/*
    ReflecTech
    ----------
    File        :    RtMouseXR.h
    Author      :    Jamie Taylor
    Last Edit   :    31/10/12
    Desc        :    A Raw Input-powered implementation of the mouse interface.
*/
#ifndef RT_MOUSE_XR_H
#define RT_MOUSE_XR_H


#include "../../PlatformIndependenceLayer/RtPlatform.h"
// the interface being implemented
#include "../HID/RtMouse.h"


// implementation specific button/key/control codes
static const U16 MouseButtonCodes[MOUSE_BUTTON_COUNT] = 
{
    // standard left, right and middle mouse buttons
    RI_MOUSE_LEFT_BUTTON_DOWN,
    RI_MOUSE_RIGHT_BUTTON_DOWN,
    RI_MOUSE_MIDDLE_BUTTON_DOWN,

    // 'X button(s)' - extra buttons outside the standard left, right and middle
    RI_MOUSE_BUTTON_4_DOWN,
    RI_MOUSE_BUTTON_5_DOWN,
};


/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
class MouseXR : public Mouse
{
public:
    // dtor
    MouseXR();
    ~MouseXR();

    // startup, reset and shutdown
    bool Startup(U32 startupParams);
    // explicitly set mouse state (key buffers, movement, wheel delta etc) to 0    
    void Reset();
    void Shutdown();

    // update/poll the mouse
    void Update(U32 updateParams);

    bool IsUp(U32 button) const;
    bool IsDown(U32 button) const;

    // get relative movement
    Movement& GetMovement() const;
    // get/set absolute position
    void SetPosition(U32 x, U32 y);
    Position& GetPosition() const;
    // get wheel delta
    U16 GetWheelDelta() const;

    // get/set cage - a cage can restrict the mouses movement to a particular region
    bool IsCaged() const;
    Cage& GetCage() const;
    void SetCage(U32 x, U32 y);

    // check device state
    bool IsConnected() const;

    void Enabled(bool state);
    bool IsEnabled() const;

private:
    // implementation specific members
    RAWINPUTDEVICE mouse;
};
/**************************************************************************************************************************/


extern "C" Mouse* CreateMouse(void *mem);
extern "C" void DestroyMouse(Mouse *p);
extern "C" U32 GetMouseMemReq();


#endif // RT_MOUSE_XR_H
