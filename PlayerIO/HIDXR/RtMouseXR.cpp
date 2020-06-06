/*
    ReflecTech
    ----------
    File        :    RtMouseXR.cpp
    Author      :    Jamie Taylor
    Last Edit   :    31/10/12
    Desc        :    A Raw Input-powered implementation of the mouse interface.
*/
#include "RtMouseXR.h"


/**************************************************************************************************************************/
// Implementation 
/**************************************************************************************************************************/
MouseState& MouseState::operator=(MouseState &rhs)
{
    buttons = rhs.buttons;
    wheelDelta = rhs.wheelDelta;

    movement.x = rhs.movement.x;
    movement.y = rhs.movement.y;

    position.x = rhs.position.x;
    position.y = rhs.position.y;

    return *this;
}

MouseState& MouseState::operator=(U32 setTo)
{
    buttons = setTo;
    wheelDelta = setTo;

    movement.x = setTo;
    movement.y = setTo;

    position.x = setTo;
    position.y = setTo;

    return *this;
}
/**************************************************************************************************************************/

MouseXR::MouseXR()
{
    currentFrame = 0;
    previousFrame = 0;

    cage.x = 0;
    cage.y = 0;
    isCaged = false;

    isConnected = true;

    isEnabled = false;
}
/**************************************************************************************************************************/

MouseXR::~MouseXR()
{
    // in case the user hasn't explicitly called Shutdown()
    Shutdown();
}
/**************************************************************************************************************************/

bool MouseXR::Startup(U32 startupParams)
{
    mouse.usUsagePage    = 0x01;
    mouse.usUsage        = 0x02;
    mouse.dwFlags        = 0x00; //RIDEV_NOLEGACY will cause the mouse to be stuck in 'busy' mode - avoid
    mouse.hwndTarget    = (HWND)startupParams;

    isEnabled = true;

    return RegisterRawInputDevices(&mouse, 1, sizeof(RAWINPUTDEVICE));
}
/**************************************************************************************************************************/

void MouseXR::Reset()
{
    currentFrame = 0;
    previousFrame = 0;

    cage.x = 0;
    cage.y = 0;
    isCaged = false;
}
/**************************************************************************************************************************/

void MouseXR::Shutdown()
{
    isEnabled = false;
}
/**************************************************************************************************************************/

void MouseXR::Update(U32 updateParams)
{
    HRAWINPUT hrw = (HRAWINPUT)updateParams;
    // update buffers & position/movement information- update here so when the mouse is disabled we don't get false positives
    previousFrame = currentFrame;
    currentFrame = 0;
    // hang onto position
    currentFrame.position.x = previousFrame.position.x;
    currentFrame.position.y = previousFrame.position.y;

    // don't update the device if it's not enabled/setup
    if(!isEnabled)
        return;

    // get input using RawInput
    char buffer[sizeof(RAWINPUT)] = {0};
    unsigned int size = sizeof(RAWINPUT);
    GetRawInputData(hrw, RID_INPUT, &buffer, &size, sizeof(RAWINPUTHEADER));

    RAWINPUT *raw = reinterpret_cast<RAWINPUT*>(buffer);
    RAWMOUSE &mouse = raw->data.mouse;

    // update button states
    // currentFrame.buttons |= mouse.usButtonFlags;
    currentFrame.buttons = mouse.usButtonFlags;
    // update scroll wheel delta
    if(mouse.usButtonFlags == RI_MOUSE_WHEEL)
        currentFrame.wheelDelta = mouse.usButtonData;

    // update relative movement
    currentFrame.movement.x = mouse.lLastX;
    currentFrame.movement.y = mouse.lLastY;
    // update absolute position
    currentFrame.position.x += mouse.lLastX;
    currentFrame.position.y += mouse.lLastY;
}
/**************************************************************************************************************************/

bool MouseXR::IsUp(U32 button) const
{
    return ((previousFrame.buttons & (MouseButtonCodes[button]) && !(currentFrame.buttons & (MouseButtonCodes[button]))));
}
/**************************************************************************************************************************/

bool MouseXR::IsDown(U32 button) const
{
    return (currentFrame.buttons & (MouseButtonCodes[button]));
}
/**************************************************************************************************************************/

Movement& MouseXR::GetMovement() const
{
    return const_cast<Movement&>(currentFrame.movement);
}
/**************************************************************************************************************************/

void MouseXR::SetPosition(U32 x, U32 y)
{
    SetCursorPos(x, y);
    currentFrame.position.x = x;
    currentFrame.position.y = y;
}
/**************************************************************************************************************************/

Position& MouseXR::GetPosition() const
{
    return const_cast<Position&>(currentFrame.position);
}
/**************************************************************************************************************************/

U16 MouseXR::GetWheelDelta() const
{
    return currentFrame.wheelDelta;
}
/**************************************************************************************************************************/

bool MouseXR::IsCaged() const
{
    return isCaged;
}
/**************************************************************************************************************************/

Cage& MouseXR::GetCage() const
{
    return const_cast<Cage&>(cage);
}
/**************************************************************************************************************************/

void MouseXR::SetCage(U32 x, U32 y)
{
    if(x == 0 && y == 0)
    {
        isCaged = false;

        cage.x = 0;
        cage.y = 0;

        return;
    }
    else
        isCaged = true;

    cage.x = x;
    cage.y = y;

    POINT p = {0};
    ClientToScreen(mouse.hwndTarget, &p);

    RECT rc = {0};

    int padding = GetSystemMetrics(SM_CYCAPTION);
    rc.top = p.x + padding;
    rc.left = p.y - padding;
    rc.bottom = rc.top + cage.x;
    rc.right = rc.left + cage.y;

    ClipCursor(&rc);
}
/**************************************************************************************************************************/

bool MouseXR::IsConnected() const
{
    return isConnected;
}
/**************************************************************************************************************************/

void MouseXR::Enabled(bool state)
{
    isEnabled = state;
}
/**************************************************************************************************************************/

bool MouseXR::IsEnabled() const
{
    return isEnabled;
}
/**************************************************************************************************************************/


// ALT CREATE & DESTROY FOR TESTING

//#include <new>
#include "../../CoreSystems/RtHeapAllocator.h" // needs a definition of placement new - TODO: FIX THIS/DECIDE ON SUBSYSTEM MEMORY ALLOCATION SYTLE

Mouse* CreateMouse(void *mem)
{
    MouseXR *temp = new(mem)MouseXR;
    return reinterpret_cast<Mouse*>(temp);
}

void DestroyMouse(Mouse *p)
{
    // destroy
    //delete p;
    reinterpret_cast<MouseXR*>(p)->~MouseXR();
}

U32 GetMouseMemReq()
{
    return sizeof(MouseXR);
}
