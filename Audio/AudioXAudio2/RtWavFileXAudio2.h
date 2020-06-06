/*
===============================================================================

    ReflecTech
    ==========
    File        :   RtWaveFileXAudio2.h
    Author      :   Jamie Taylor
    Last Edit   :   25/03/13
    Desc        :   An XAudio2 implementationof the basic static wave file class. Adopting a DOOM 3 style syntax.

===============================================================================
*/


#ifndef RT_WAV_FILE_XAUDIO2_H
#define RT_WAV_FILE_XAUDIO2_H


#include "../../PlatformIndependenceLayer/RtPlatform.h"
#include "../../CoreSystems/RtMemoryCommon.h"

// needed to test factory functions
#include "../../CoreSystems/RtHeapAllocator.h"

#include "../RtWavFile.h"
#include <XAudio2.h>
#include <X3DAudio.h>
// for vectors used by the 3d audio
#include <D3DX10math.h>


/*
===============================================================================

XAudio2 powered Wave file class

===============================================================================
*/
#define FOURCC_ID_RIFF 'FFIR'
#define FOURCC_ID_DATA 'atad'
#define FOURCC_ID_FMT  ' tmf'
#define FOURCC_ID_WAVE 'EVAW'
#define FOURCC_ID_XWMA 'AMWX'


class WavFileXAudio2 : public WavFile, public IXAudio2VoiceCallback {
public:
                            WavFileXAudio2( void );
                            ~WavFileXAudio2( void );

    bool                    Load( const I8 *fileName );
    void                    UnLoad( void );

    void                    Play( void );
    void                    Pause( void );
    void                    Stop( void );

                            // loopStart and loopDuration expect a number of seconds
    void                    SetLoopingParams( U32 loopStart, U32 loopDuration, U32 loopCount, bool immediateEffect = true);

                            // within the range 0.0f - 1.0f
    void                    SetVolume( F32 vol );
    F32                     GetVolume( void ) const;

    bool                    IsLoaded( void ) const;
    bool                    IsPlaying( void ) const;

    // void Set3d( bool 3d );
    // bool Is3d( ) const;
    // set 3d position, front and up vectors
    // get position, front and up vectors?

                            // implementation specific helpers
    WAVEFORMATEXTENSIBLE*    GetFormatData( void ) const;
    X3DAUDIO_EMITTER*       Get3dEmitter( void ) const;
    IXAudio2SourceVoice*    GetSourceVoice( void ) const;

private:
                            // private implementation specific helpers, used to parse a PCM wav file
    bool                    FindChunk( U32 fourcc, U32 &chunkSize, U32 &chunkPosition );
    U32                     ReadChunk( void *buffer, U32 bytesToRead, U32 bufferOffset );

                            // private, implementation specific members
    HANDLE                  audioFile;
    WAVEFORMATEXTENSIBLE    audioFileFormat;
    XAUDIO2_BUFFER          audioFileBuffer;
    IXAudio2SourceVoice *   sourceVoice;

                            // used for 3D audio, an audio file can represent a sound emitter in the world
    // bool                 is3d;
    X3DAUDIO_EMITTER        emitter;

                            // local alloator
    HeapAllocator<void>     allocator;

                            // private implementation specific helpers; implemented XAudio2 callback methods
    void __stdcall          OnStreamEnd( void );
    void __stdcall          OnVoiceProcessingPassEnd( void );
    void __stdcall          OnVoiceProcessingPassStart( U32 SamplesRequired );
    void __stdcall          OnBufferEnd( void * pBufferContext );
    void __stdcall          OnBufferStart( void * pBufferContext );
    void __stdcall          OnLoopEnd( void * pBufferContext );
    void __stdcall          OnVoiceError( void * pBufferContext, HRESULT Error );
};


// ALT CREATE & DESTROY FOR TESTING
extern "C" WavFile * CreateWavFile( void *mem );
extern "C" void      DestroyWavFile( WavFile *p );
extern "C" U32       GetWavFileMemoryRequirement( void );


#endif // RT_WAV_FILE_XAUDIO2_H
