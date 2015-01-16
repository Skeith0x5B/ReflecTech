/*
	ReflecTech
	----------
	File		:	RtButtonSequenceDetector.h
	Author		:	Jamie Taylor
	Last Edit	:	27/10/12
	Desc		:	Looks for a chord, a chord is a particular sequence of buttons/inputs.
*/  
#ifndef RT_BUTTON_SEQUENCE_DETECTOR_H
#define RT_BUTTON_SEQUENCE_DETECTOR_H


#include "../PlatformIndependenceLayer/RtPlatform.h"
#include "../PlatformIndependenceLayer/RtTimer.h"


//namespace ReflecTech { namespace PlayerIO {


	/**************************************************************************************************************************/
	// Definition
	/**************************************************************************************************************************/
	template<class T>
	class ButtonSequenceDetector
	{
	public:
		ButtonSequenceDetector(T *_device, U32 *sequence, U32 _ButtonSequenceDetectorLength, F32 timeAllowed);
		~ButtonSequenceDetector();

		// return true when the button sequence is complete, false if it's not complete yet or simply resets
		bool Update();

	private:
		// the ButtonSequenceDetector itself
		U32 *buttonSequence;
		// information about it/progress
		U32  nextButton;
		U16  ButtonSequenceDetectorLength; 
		U16  ButtonSequenceDetectorElapsed;

		// timer
		Timer timer;
		// milliseconds
		F32 startTime;
		F32 timeWindow; 

		// the actual IO device to listen to
		T *device;
	};
	/**************************************************************************************************************************/


	/**************************************************************************************************************************/
	// Implementation 
	/**************************************************************************************************************************/
	template<class T>
	ButtonSequenceDetector<T>::ButtonSequenceDetector(T *_device, U32 *sequence, U32 _ButtonSequenceDetectorLength, F32 timeAllowed)
	{
		device = _device;

		buttonSequence = sequence;
		ButtonSequenceDetectorLength = _ButtonSequenceDetectorLength;
		timeWindow = timeAllowed;

		startTime = 0.0f;
		nextButton = 0;
	}
	/**************************************************************************************************************************/

	template<class T>
	ButtonSequenceDetector<T>::~ButtonSequenceDetector()
	{
		device = 0;
		buttonSequence = 0;
	}
	/**************************************************************************************************************************/

	template<class T>
	bool ButtonSequenceDetector<T>::Update()
	{
		if(device->IsDown(buttonSequence[nextButton]))
		{
			if(nextButton == 0)
			{
				// first element in chord, start the timer
				timer.Reset();
				startTime = timer.GetMillisecondsF();
				++nextButton;
			}
			else
			{
				// move along the chord, provided we're within the time window
				F32 currTime = timer.GetMillisecondsF();

				if(currTime - startTime < timeWindow)
				{
					// next button
					++nextButton;

					// was this the final input needed?
					if(nextButton == ButtonSequenceDetectorLength)
					{
						// reset for next time
						nextButton = 0; 
						return true;
					}
				}
				else
				{
					// timed out - no need to explicitly reset the timer, it'll reset next time the chord starts
					nextButton = 0;
				}
			}
		}

		// timed-out or the chord simply isn't complete yet
		return false;
	}
	/**************************************************************************************************************************/


//} // ReflecTech
//} // PlayerIO


#endif // RT_BUTTON_SEQUENCE_DETECTOR_H