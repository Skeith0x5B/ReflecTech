/*
    ReflecTech
    ----------
    File        :    RtGamepad.h
    Author      :    Jamie Taylor
    Last Edit   :    27/10/12
    Desc        :    The interface for a gamepad. An implementation needs to be present to use it with the engine.
                     PIMPL is favoured because you can add extra implementation-specific helpers without updating/adding boiler-plate.

                     The startup & updateParams are intended to allow any implementation to pass specific information needed for polling/updating.
                     The idea is to pass some integer value if that's all that's required, otherwise the implementation could define a struct containing
                     information it needs to update/poll the controller; in this case the parameter could be used to pass the memory address of a struct instance.
*/  
#ifndef RT_GAMEPAD_H
#define RT_GAMEPAD_H


#include "../../PlatformIndependenceLayer/RtPlatform.h"
#include "../../PlatformIndependenceLayer/RtTimer.h"


// used for sqrt - move?
#include <math.h>


// Intended to be used for indexing into an OS/API specific keyCode array
// REGARDLESS of exact implementation, engine/application code should be able to use these enum values and be shielded from OS/API specifics
enum RT_GAMEPAD
{
    // center
    START,
    BACK_PAUSE,

    // left pad ("dpad", < ^ > etc)
    LEFT_PAD_UP,
    LEFT_PAD_DOWN,
    LEFT_PAD_LEFT,
    LEFT_PAD_RIGHT,

    // right pad (A,B, O,X etc)
    RIGHT_PAD_UP,
    RIGHT_PAD_DOWN,
    RIGHT_PAD_LEFT,
    RIGHT_PAD_RIGHT,

    // bumpers/shoulder buttons (LB, RB, L1, R1)
    LEFT_SHOULDER,
    RIGHT_SHOULDER,

    // triggers (LT, RT, L2, R2)
    LEFT_TRIGGER,
    RIGHT_TRIGGER,

    // stick
    LEFT_STICK,
    // analogue stick quadrants in clockwise order
    LEFT_STICK_UPPER_LEFT_QUADRANT,
    LEFT_STICK_UPPER_RIGHT_QUADRANT,
    LEFT_STICK_LOWER_RIGHT_QUADRANT,
    LEFT_STICK_LOWER_LEFT_QUADRANT,
    // analogue stick down (L3, R3 etc)
    LEFT_STICK_BUTTON,

    // stick
    RIGHT_STICK,
    // analogue stick quadrants in clockwise order
    RIGHT_STICK_UPPER_LEFT_QUADRANT,
    RIGHT_STICK_UPPER_RIGHT_QUADRANT,
    RIGHT_STICK_LOWER_RIGHT_QUADRANT,
    RIGHT_STICK_LOWER_LEFT_QUADRANT,
    // analogue stick down (L3, R3 etc)
    RIGHT_STICK_BUTTON,

    // used for array indexing
    GAMEPAD_BUTTON_COUNT,
};


// analog stick direction represented as a vector
struct AnalogStick
{
    I16 x;
    I16 y;
};

// normalised version
struct NormalisedAnalogStick
{
    F32 x;
    F32 y;
};


// store gamepad state
struct GamepadState
{
    // digital buttons
    I16 buttons;

    // analog triggers
    I16 leftTrigger;
    I16 rightTrigger;

    // analog sticks, separate variables for offset along X and Y axis
    AnalogStick leftStick;
    AnalogStick rightStick;

    GamepadState& operator=(GamepadState &rhs){
        buttons = rhs.buttons;

        leftTrigger = rhs.leftTrigger;
        rightTrigger = rhs.rightTrigger;

        leftStick.x = rhs.leftStick.x;
        leftStick.y = rhs.leftStick.y;
        rightStick.x = rhs.rightStick.x;
        rightStick.y = rhs.rightStick.y;

        return *this;
    }

    GamepadState& operator=(U32 setTo) {
        buttons = setTo;

        leftTrigger = setTo;
        rightTrigger = setTo;

        leftStick.x = setTo;
        leftStick.y = setTo;
        rightStick.x = setTo;
        rightStick.y = setTo;

        return *this;
    }
};

// lnk2005
//GamepadState& GamepadState::operator=(GamepadState &rhs)
//{
//    buttons = rhs.buttons;
//
//    leftTrigger = rhs.leftTrigger;
//    rightTrigger = rhs.rightTrigger;
//
//    leftStick.x = rhs.leftStick.x;
//    leftStick.y = rhs.leftStick.y;
//    rightStick.x = rhs.rightStick.x;
//    rightStick.y = rhs.rightStick.y;
//
//    return *this;
//}

// lnk2005
//GamepadState& GamepadState::operator=(U32 setTo)
//{
//    buttons = setTo;
//
//    leftTrigger = setTo;
//    rightTrigger = setTo;
//
//    leftStick.x = setTo;
//    leftStick.y = setTo;
//    rightStick.x = setTo;
//    rightStick.y = setTo;
//
//    return *this;
//}


/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
class Gamepad
{
public:
    // dtor
    virtual ~Gamepad() {};

    // startup, reset and shutdown
    virtual bool Startup(U32 startupParams = 0) = 0;
    // explicitly set key buffers/gamepad state to 0
    virtual void Reset() = 0; 
    virtual void Shutdown() = 0;

    // update the gamepad state
    virtual void Update(U32 updateParams = 0) = 0;

    // standard digital buttons (X, Y / X, O etc...) analog stick quadrants should also be compatible with these functions
    virtual bool IsUp(U32 button) const = 0;
    virtual bool IsDown(U32 button) const = 0;
    // think running through the environment or accelerating a craft, you don't want the action to stop due to previousFrame/currentFrame state differences
    // that the two above functions enforce
    virtual bool IsBeingHeld(U32 button) const  = 0;

    // triggers
    virtual I16 GetTriggerValue(U32 trigger) const = 0;
    virtual F32 GetNormalisedTriggerValue(U32 trigger) const = 0;

    // analog sticks
    virtual AnalogStick& GetStickPosition(U32 stick) const = 0;
    virtual NormalisedAnalogStick GetNormalisedStickPosition(U32 stick) const = 0;

    // vibrate the controller
    virtual void SetVibration(I16 left, I16 right, F32 duration) = 0;

    // check device state
    virtual bool IsConnected() const = 0;
    virtual void Enabled(bool state) = 0;
    virtual bool IsEnabled() const = 0;

protected:
    // all implementations will inherit these members and then add their own implementation specific private members
    bool isConnected;
    bool isEnabled;

    GamepadState currentFrame;
    GamepadState previousFrame;

    //Timer timer;
    F32 vibrationStartTime;
    F32 vibrationDuration;
};
/**************************************************************************************************************************/


#endif // RT_GAMEPAD_H
