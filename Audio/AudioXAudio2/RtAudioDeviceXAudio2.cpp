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


#include "RtAudioDeviceXAudio2.h"


/*
================
AudioDeviceXAudio2::AudioDeviceXAudio2
================
*/
AudioDeviceXAudio2::AudioDeviceXAudio2( ) {
    listHead = NULL;
    listHead = NULL;

    audioEngine    = NULL;
    masteringVoice = NULL;

    coefficientsMatrix = NULL;

    ZeroMemory( &X3DInstance, sizeof( X3DAUDIO_HANDLE ) );
    ZeroMemory( &listener, sizeof( X3DAUDIO_LISTENER ) );
    ZeroMemory( &dspSettings, sizeof( X3DAUDIO_DSP_SETTINGS ) );

    isRunning = false;

    static DynamicPoolAllocator<AudioDeviceView> allctr( sizeof( AudioDeviceView ), 1, 1 );
    memAllctr = &allctr;

    static HeapAllocator<void> hAllctr;
    heapAllctr = &hAllctr;
}

/*
================
AudioDeviceXAudio2::~AudioDeviceXAudio2
================
*/
AudioDeviceXAudio2::~AudioDeviceXAudio2( void ) {
    if( isRunning == true ) {
        Shutdown( );
    }
}

/*
================
AudioDeviceXAudio2::GetSingletonReference
================
*/
AudioDevice& AudioDeviceXAudio2::GetSingletonReference( void ) {
    return *singletonInstance;
}

/*
================
AudioDeviceXAudio2::GetSingletonPointer
================
*/
AudioDevice* AudioDeviceXAudio2::GetSingletonPointer( void ) {
    return singletonInstance;
}

/*
================
AudioDeviceXAudio2::Start
================
*/
bool AudioDeviceXAudio2::Start( void ) {
    // start COM
    CoInitializeEx( NULL, COINIT_MULTITHREADED );

    // create xAudio2 engine instance
    if( FAILED( XAudio2Create( &audioEngine, XAUDIO2_DEBUG_ENGINE, XAUDIO2_DEFAULT_PROCESSOR ) ) ) {
        CoUninitialize( );
        return false;
    }
    
    // create mastering voice
    if( FAILED( audioEngine->CreateMasteringVoice( &masteringVoice ) ) ) {
        audioEngine->Release( );
        CoUninitialize( );
        return false;
    }

    // setup 3d audio
    XAUDIO2_DEVICE_DETAILS deviceDetails = { 0 };
    audioEngine->GetDeviceDetails( 0, &deviceDetails );
    X3DAudioInitialize( deviceDetails.OutputFormat.dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, X3DInstance );

    listener.Position    = D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
    listener.OrientFront = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
    listener.OrientTop   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

    dspSettings.SrcChannelCount = 1;
    dspSettings.DstChannelCount = deviceDetails.OutputFormat.Format.nChannels;

    coefficientsMatrix = reinterpret_cast<F32*>( heapAllctr->Allocate( sizeof( F32 ) * ( dspSettings.SrcChannelCount * dspSettings.DstChannelCount ) ) );
    dspSettings.pMatrixCoefficients = coefficientsMatrix;

    isRunning = true;
    return true;
}

/*
================
AudioDeviceXAudio2::Shutdown
================
*/
void AudioDeviceXAudio2::Shutdown( void ) {
    // shutdown mixers
    ShutdownMixers( );

    // get rid of the audio files
    UnloadAudioFiles( );

    // free memory for 3d coefficients matrix
    if( coefficientsMatrix != NULL ) {
        heapAllctr->DeAllocate( coefficientsMatrix );
        coefficientsMatrix = NULL;
    }

    // destroy mastering voice
    if( masteringVoice != NULL ) {
        masteringVoice->DestroyVoice( );
        masteringVoice = NULL;
    }

    // destroy xAudio2 engine instance
    if( audioEngine != NULL ) {
        audioEngine->StopEngine( );
        audioEngine->Release( );
        CoUninitialize( );
    }

    isRunning = false;
}

/*
================
AudioDeviceXAudio2::GetMixerCount
================
*/
U32 AudioDeviceXAudio2::GetMixerCount( void ) const {
    U32 attachedCount = 0;

    AudioDeviceView *p = listHead;
    while( p != NULL ) {
        if( p->mixer != NULL ) {
            ++attachedCount;
        }
        p = p->next;
    }

    return attachedCount;
}

/*
================
AudioDeviceXAudio2::GetActiveMixerCount
================
*/
U32 AudioDeviceXAudio2::GetActiveMixerCount( void ) const {
    U32 activeCount = 0;

    AudioDeviceView *p = listHead;
    while( ( p != NULL )) {
        if( ( p->mixer != NULL ) && ( p->mixer->IsRunning( ) == true )  ) {
            ++activeCount;
        }
        p = p->next;
    }

    return activeCount;
}

/*
================
AudioDeviceXAudio2::GetAudioFileCount
================
*/
U32 AudioDeviceXAudio2::GetAudioFileCount( void ) const {
    U32 attachedCount = 0;

    AudioDeviceView *p = listHead;
    while( p != NULL ) {
        if( p->file != NULL ) {
            ++attachedCount;
        }
        p = p->next;
    }

    return attachedCount;
}

/*
================
AudioDeviceXAudio2::GetActiveAudioFileCount
================
*/
U32 AudioDeviceXAudio2::GetActiveAudioFileCount( void ) const {
    U32 activeCount = 0;

    AudioDeviceView *p = listHead;
    while( p != NULL ) {
        if( ( p->file != NULL ) && ( p->file->IsPlaying( ) == true ) ) {
            ++activeCount;
        }
        p = p->next;
    }

    return activeCount;
}

/*
================
AudioDeviceXAudio2::Play
================
*/
void AudioDeviceXAudio2::Play( void ) {
    AudioDeviceView *p = listHead;
    while( p != NULL ) {
        if( p->file != NULL && p->file->IsPlaying( ) == false ) {
            p->file->Play( );
        }
        p = p->next;
    }
}

/*
================
AudioDeviceXAudio2::Pause
================
*/
void AudioDeviceXAudio2::Pause( void ) {
    AudioDeviceView *p = listHead;
    while( p != NULL ) {
        if( p->file != NULL && p->file->IsPlaying( ) == true ) {
            p->file->Pause( );
        }
        p = p->next;
    }
}

/*
================
AudioDeviceXAudio2::Stop
================
*/
void AudioDeviceXAudio2::Stop( void ) {
    AudioDeviceView *p = listHead;
    while( p != NULL ) {
        if( p->file != NULL && p->file->IsPlaying( ) == true ) {
            p->file->Stop( );
        }
        p = p->next;
    }
}

/*
================
AudioDeviceXAudio2::SetLoopingParams
================
*/
void AudioDeviceXAudio2::SetLoopingParams( U32 loopStart, U32 loopDuration, U32 loopCount ) {
    AudioDeviceView *p = listHead;
    while( p != NULL ) {
        if( ( p->file != NULL ) ) {
            p->file->SetLoopingParams( loopStart, loopDuration, loopCount );
        }
        p = p->next;
    }
}

/*
================
AudioDeviceXAudio2::SetVolume
================
*/
void AudioDeviceXAudio2::SetVolume( F32 vol ) {
    AudioDeviceView *p = listHead;
    while( p != NULL ) {
        if( ( p->file != NULL ) ) {
            p->file->SetVolume( vol );
        }
        p = p->next;
    }
}

/*
================
AudioDeviceXAudio2::Update3DAudio
================
*/
void AudioDeviceXAudio2::Update3DAudio( F32 xDelta, F32 zDelta, F32 pitchDelta, F32 yawDelta ) {
    U32 flags = X3DAUDIO_CALCULATE_MATRIX;

    XMVECTOR zRef = XMVectorSet( xDelta, 0.0f, zDelta, 1.0f );
    XMMATRIX rota = XMMatrixRotationY( yawDelta );
    zRef = XMVector3Transform( zRef, rota );
    
    XMFLOAT3 zRefTransformed;
    XMStoreFloat3( &zRefTransformed, zRef );
    /*if( zRefTransformed.x || zRefTransformed.y || zRefTransformed.z != 0.0f ) {
        listener.OrientFront.x = zRefTransformed.x;
        listener.OrientFront.y = zRefTransformed.y;
        listener.OrientFront.z = zRefTransformed.z;
        */
        listener.Position.x += zRefTransformed.x;
        listener.Position.z += zRefTransformed.z;
    //}

    AudioDeviceView *p = listHead;
    while( p != NULL ) {
        if( ( p->file != NULL ) ) { // && ( file->Is3d( ) == true )
            // get emitter from wav file
            X3DAUDIO_EMITTER *emitter = p->file->Get3dEmitter( );
            IXAudio2SourceVoice *sourceVoice = p->file->GetSourceVoice( );

            X3DAudioCalculate( X3DInstance, &listener, emitter, flags, &dspSettings );

            sourceVoice->SetOutputMatrix( masteringVoice, /*dspSettings.SrcChannelCount*/ 2,
                                          dspSettings.DstChannelCount, coefficientsMatrix, 0 );
        }
        p = p->next;
    }
}

/*
================
AudioDeviceXAudio2::ShutdownMixers
================
*/
void AudioDeviceXAudio2::ShutdownMixers( void ) {
    AudioDeviceView *p = listHead;
    while( p != NULL ) {
        if( ( p->mixer != NULL ) ) {
            p->mixer->Shutdown( );
        }
        p = p->next;
    }
}

/*
================
AudioDeviceXAudio2::UnloadAudioFiles
================
*/
void AudioDeviceXAudio2::UnloadAudioFiles( void ) {
    AudioDeviceView *p = listHead;
    while( p != NULL ) {
        if( ( p->file != NULL ) ) {
            p->file->UnLoad( );
        }
        p = p->next;
    }
}

/*
================
AudioDeviceXAudio2::RegisterMixer
================
*/
void AudioDeviceXAudio2::RegisterMixer( Mixer *mixer ) {
    MixerXAudio2 *mix = reinterpret_cast<MixerXAudio2*>( mixer );

    if( listHead == NULL ) {
        listHead = memAllctr->Allocate( );
        listHead->mixer = mix;
        listHead->next = NULL;
    }
    else {
        AudioDeviceView *thisFile = memAllctr->Allocate( );
        thisFile->mixer = listHead->mixer;
        thisFile->next  = listHead->next;
        thisFile->file  = NULL; 

        listHead->mixer = mix;
        listHead->next = thisFile;
    }
    listHead->file = NULL;
}

/*
================
AudioDeviceXAudio2::UnRegisterMixer
================
*/
void AudioDeviceXAudio2::UnRegisterMixer( Mixer *mixer ) {
    MixerXAudio2 *mix = reinterpret_cast<MixerXAudio2*>( mixer );

    AudioDeviceView *p = listHead;
    while( p != NULL ) {
        if( p->mixer == mix ) {
            memAllctr->DeAllocate( p );
            break;
        }
        p = p->next;
    }
}

/*
================
AudioDeviceXAudio2::RegisterWavFile
================
*/
void AudioDeviceXAudio2::RegisterWavFile( WavFile *file ) {
    WavFileXAudio2 *wav = reinterpret_cast<WavFileXAudio2*>( file );

    if( listHead == NULL ) {
        listHead = memAllctr->Allocate( );
        listHead->file = wav;
        listHead->next = NULL;
    }
    else {
        AudioDeviceView *thisFile = memAllctr->Allocate( );
        thisFile->file = listHead->file;
        thisFile->next = listHead->next;
        thisFile->mixer = NULL; 

        listHead->file = wav;
        listHead->next = thisFile;
    }
    listHead->mixer = NULL;
}

/*
================
AudioDeviceXAudio2::UnRegisterWavFile
================
*/
void AudioDeviceXAudio2::UnRegisterWavFile( WavFile *file ) {
    WavFileXAudio2 *wav = reinterpret_cast<WavFileXAudio2*>( file );

    AudioDeviceView *p = listHead;
    while( p != NULL ) {
        if( p->file == wav ) {
            memAllctr->DeAllocate( p );
            break;
        }
        p = p->next;
    }
}

/*
================
AudioDeviceXAudio2::GetXAudioEngine
================
*/
IXAudio2* AudioDeviceXAudio2::GetXAudioEngine( void ) const {
    return audioEngine;
}


// ALT CREATE & DESTROY FOR TESTING
AudioDevice* CreateAudioDevice( void *mem ) {
    AudioDeviceXAudio2 *temp = new( mem )AudioDeviceXAudio2;
    return reinterpret_cast<AudioDevice*>( temp );
}

void DestroyAudioDevice( AudioDevice *p ) {
    // destroy
    //delete p;
    reinterpret_cast<AudioDeviceXAudio2*>( p )->~AudioDeviceXAudio2( );
}

U32 GetAudioDeviceMemoryRequirement( void ) {
    return sizeof( AudioDeviceXAudio2 );
}
