/*
===============================================================================

	ReflecTech
	==========
	File		:	RtAudioDevice.h
	Author		:	Jamie Taylor
	Last Edit	:	31/03/13
	Desc		:	Singleton audio device/manager, can control all wav files and mixers.
					All wav files and mixers automatically register themselves with the
					device/manager for tracking via private functions.

===============================================================================
*/


#ifndef RT_AUDIO_DEVICE_H
#define RT_AUDIO_DEVICE_H


#include "../CoreSystems/RtSingleton.h"


class WavFile;
class Mixer;


/*
===============================================================================

Audio device class

===============================================================================
*/
class AudioDevice : public Singleton<AudioDevice> {
public:
						 AudioDevice( ) { };
	virtual				 ~AudioDevice( void ) { };

						 // singleton related
	static AudioDevice& GetSingletonReference( void );
	static AudioDevice* GetSingletonPointer( void );

	virtual bool		 Start( void ) = 0;
    virtual void		 Shutdown( void ) = 0;

	virtual U32			 GetAudioFileCount( void ) const = 0;
	virtual U32			 GetActiveAudioFileCount( void ) const = 0;
	virtual U32			 GetMixerCount( void ) const = 0;
	virtual U32			 GetActiveMixerCount( void ) const = 0;

						 // play, pause, stop all audio files
	virtual void		 Play( void ) = 0;
	virtual void		 Pause( void ) = 0;
	virtual void		 Stop( void ) = 0;

						 // set looping params for all audio files
	virtual void		 SetLoopingParams( U32 loopStart, U32 loopDuration, U32 loopCount ) = 0;
						 // set volume for all audio files (range: 0.0f - 1.0f)
	virtual void		 SetVolume( F32 vol ) = 0;

	virtual void		 Update3DAudio( F32 xDelta, F32 zDelta, F32 pitchDelta, F32 yawDelta ) = 0; 

	virtual void		 ShutdownMixers( void ) = 0;
	virtual void		 UnloadAudioFiles( void ) = 0;
	
						 // TEMP
	virtual void		 RegisterMixer( Mixer *mixer ) = 0;
	virtual void		 UnRegisterMixer( Mixer *mixer ) = 0;
    virtual void		 RegisterWavFile( WavFile *file ) = 0;
    virtual void		 UnRegisterWavFile( WavFile *file ) = 0;

protected:
	bool				 isRunning;
};


template<> AudioDevice *Singleton<AudioDevice>::singletonInstance = NULL;


#endif // RT_AUDIO_DEVICE_H