/*
    ReflecTech
    ----------
    File        :    RtButtonTapCounter.h
    Author      :    Jamie Taylor
    Last Edit   :    27/10/12
    Desc        :    Count how many times a button/input is being pressed/tapped within quick succession.
*/ 
#ifndef RT_TAP_COUNTER_H
#define RT_TAP_COUNTER_H


#include "../PlatformIndependenceLayer/RtPlatform.h"
#include "../PlatformIndependenceLayer/RtTimer.h"


//namespace ReflecTech { namespace PlayerIO {


    /**************************************************************************************************************************/
    // Definition
    /**************************************************************************************************************************/
    template<class T>
    class ButtonTapDetector
    {
    public:
        ButtonTapDetector(T *_device, U32 button, F32 _timeAllowed);
        ~ButtonTapDetector();

        U32 Update();

    private:
        // button to watch for
        U32 buttonToWatch;
        // how many times has the button been pressed in quick succession?
        U32 count;

        // timer instance
        Timer timer;
        F32 lastTime;
        F32 timeAllowed;

        // the actual IO device
        T *device;
    };
    /**************************************************************************************************************************/


    /**************************************************************************************************************************/
    // Implementation 
    /**************************************************************************************************************************/
    template<class T>
    ButtonTapDetector<T>::ButtonTapDetector(T *_device, U32 button, F32 _timeAllowed)
    {
        device = _device;
        buttonToWatch = button;

        count = 0;
        lastTime = 0.0f;
        timeAllowed = _timeAllowed;
    }
    /**************************************************************************************************************************/

    template<class T>
    ButtonTapDetector<T>::~ButtonTapDetector()
    {
        device = 0;
    }
    /**************************************************************************************************************************/

    template<class T>
    U32 ButtonTapDetector<T>::Update()
    {
        // is the the button being pressed?
        if(device->IsDown(buttonToWatch))
        {
            // first time the button's been pressed?
            if(count == 0)
            {
                ++count;
                lastTime = timer.GetMilliseconds();
            }
            else
            {
                // were we fast enough?
                F32 currTime = timer.GetMilliseconds();

                if(currTime - lastTime < timeAllowed)
                {
                    // yes, count this
                    ++count;
                    lastTime = currTime;
                }
                else
                {
                    // too slow
                    count = 0;
                }
            }
        }

        // return the number of times the buttons been pressed
        return count;
    }
    /**************************************************************************************************************************/


//} // ReflecTech
//} // PlayerIO


#endif // RT_TAP_COUNTER_H
