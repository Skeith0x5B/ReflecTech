/*
===============================================================================

    ReflecTech
    ==========
    File        :   RtAudioDeviceXAudio2.h
    Author      :   Jamie Taylor
    Last Edit   :   31/03/13
    Desc        :   XAudio2 implementation of the audio device.

===============================================================================
*/


#ifndef RT_AUDIO_DEVICE_XAUDIO2_H
#define RT_AUDIO_DEVICE_XAUDIO2_H


#include "../RtAudioDevice.h"
#include "RtMixerXAudio2.h"
#include "RtWavFileXAudio2.h"

#include "../../CoreSystems/RtHeapAllocator.h"

// for vector & matrix ops
#include <xnamath.h>


/*
===============================================================================

XAudio2 powered audio manager class

===============================================================================
*/
class AudioDeviceXAudio2 : AudioDevice {
public:
                                            AudioDeviceXAudio2( void );
                                            ~AudioDeviceXAudio2( void );

                                            // singleton related
    static AudioDevice&                     GetSingletonReference( void );
    static AudioDevice*                     GetSingletonPointer( void );

    bool                                    Start( void );
    void                                    Shutdown( void );

    U32                                     GetMixerCount( void ) const;
    U32                                     GetActiveMixerCount( void ) const;
    U32                                     GetAudioFileCount( void ) const;
    U32                                     GetActiveAudioFileCount( void ) const;

                                            // play, pause, stop all audio files
    void                                    Play( void );
    void                                    Pause( void );
    void                                    Stop( void );

                                            // set looping params for all audio files
    void                                    SetLoopingParams( U32 loopStart, U32 loopDuration, U32 loopCount );
                                            // set volume for all audio files (range: 0.0f - 1.0f)
    void                                    SetVolume( F32 vol );

    void                                    Update3DAudio( F32 xDelta, F32 zDelta, F32 pitchDelta, F32 yawDelta );

    void                                    ShutdownMixers( void );
    void                                    UnloadAudioFiles( void );

                                            // implementation helper 
    IXAudio2*                               GetXAudioEngine( void ) const;

                                            // TEMP
    void                                    RegisterMixer( Mixer *mixer );
    void                                    UnRegisterMixer( Mixer *mixer );
    void                                    RegisterWavFile( WavFile *file );
    void                                    UnRegisterWavFile( WavFile *file );

private:
                                            struct AudioDeviceView {
                                                AudioDeviceView( void ) { ZeroMem( this, sizeof( this ) ); }

                                            WavFileXAudio2	 * file;
                                                MixerXAudio2    * mixer;
                                                AudioDeviceView * next;
                                            };

    AudioDeviceView                        * listHead;
    HeapAllocator<void>                    * heapAllctr;
    DynamicPoolAllocator<AudioDeviceView>  * memAllctr;

    IXAudio2                               * audioEngine;
    IXAudio2MasteringVoice                 * masteringVoice;

                                            // used for 3D audio, manager "represents" the player (the listener)
    X3DAUDIO_HANDLE                         X3DInstance;
    X3DAUDIO_LISTENER                       listener;
    X3DAUDIO_DSP_SETTINGS                   dspSettings;
    F32                                   * coefficientsMatrix;
};


// ALT CREATE & DESTROY FOR TESTING
extern "C" AudioDevice * CreateAudioDevice( void *mem );
extern "C" void          DestroyAudioDevice( AudioDevice *p );
extern "C" U32           GetAudioDeviceMemoryRequirement( void );


#endif // RT_AUDIO_DEVICE_XAUDIO2_H
