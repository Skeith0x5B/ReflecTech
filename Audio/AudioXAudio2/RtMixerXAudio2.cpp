/*
===============================================================================

    ReflecTech
    ==========
    File        :   RtMixer.cpp
    Author      :   Jamie Taylor
    Last Edit   :   31/03/13
    Desc        :   An xAudio2 powered mixer class implementation.

===============================================================================
*/


#include "RtMixerXAudio2.h"
#include "RtAudioDeviceXAudio2.h"


/*
================
MixerXAudio2::MixerXAudio2
================
*/
MixerXAudio2::MixerXAudio2( void ) {
    isRunning = false;
    listHead = NULL;
    submixVoice = NULL;
    // TEMPORARY HELPER
    tempHelper = NULL;

    static DynamicPoolAllocator<AudioFileMixerView> allctr( sizeof( AudioFileMixerView ), 1, 1 );
    memAllctr = &allctr;

    // register with manager here
    AudioDeviceXAudio2::GetSingletonReference( ).RegisterMixer( reinterpret_cast<Mixer*>( this ) );
}

/*
================
MixerXAudio2::~MixerXAudio2
================
*/
MixerXAudio2::~MixerXAudio2( void ) {
    // unregister with manager here
    AudioDeviceXAudio2::GetSingletonReference( ).UnRegisterMixer( reinterpret_cast<Mixer*>( this ) );

    if( isRunning == true ) {
        Shutdown( );
    }
}

/*
================
MixerXAudio2::Start
================
*/
bool MixerXAudio2::Start( void ) {
    // must have at least one wav file attached since we need channel information and such to start up the 
    // submix voice, the tempHelper member will be removed
    if( tempHelper == NULL  || listHead == NULL || listHead->file == NULL || isRunning == true ) {
        return false;
    }

    IXAudio2 *audioEngine = reinterpret_cast<IXAudio2*>( tempHelper );
    const WAVEFORMATEXTENSIBLE *format = reinterpret_cast<WAVEFORMATEXTENSIBLE*>( listHead->file->GetFormatData( ) );

    // else create submix voice using info from first audio file (list head)
    if( FAILED( audioEngine->CreateSubmixVoice( &submixVoice, format->Format.nChannels, format->Format.nSamplesPerSec, 0, 0, NULL, NULL) ) ) {
        return false;
    }

    isRunning = true;
    return true;
}

/*
================
MixerXAudio2::Shutdown
================
*/
void MixerXAudio2::Shutdown( void ) {
    // clear out the list of attached audio file
    if( listHead != NULL ) {
        AudioFileMixerView *p = listHead;
        while( listHead != 0 ) {
            listHead->file = 0;
            p = listHead;
            listHead = listHead->next;
            memAllctr->DeAllocate( p );
        }
    }

    if( submixVoice != NULL ) {
        submixVoice->DestroyVoice( );
        submixVoice = NULL;
    }

    isRunning = false;
}

/*
================
MixerXAudio2::AttachAudioFile
================
*/
void MixerXAudio2::AttachAudioFile( WavFile *audioFile ) {
    WavFileXAudio2 *wav = reinterpret_cast<WavFileXAudio2*>( audioFile );

    if( listHead == NULL ) {
        listHead = memAllctr->Allocate( );
        listHead->file = wav;
        listHead->next = NULL;
    }
    else {
        AudioFileMixerView *thisFile = memAllctr->Allocate( );
        thisFile->file = listHead->file;
        thisFile->next = listHead->next;

        listHead->file = wav;
        listHead->next = thisFile;
    }
}

/*
================
MixerXAudio2::DetachAudioFile
================
*/
void MixerXAudio2::DetachAudioFile( WavFile *audioFile ) {
    WavFileXAudio2 *wav = reinterpret_cast<WavFileXAudio2*>( audioFile );

    AudioFileMixerView *p = listHead;
    AudioFileMixerView *q = p;

    while( p != 0 ) {
        if( p->file != NULL && p->file == wav ) {
            q->next = p->next;
            p->next = 0;
            p->file = 0;

            memAllctr->DeAllocate( p );
            p = 0;
            break;
        }
        q = p;
        p = p->next;
    }
}

void MixerXAudio2::DetachAllAudioFiles( void ) {
    AudioFileMixerView *p = listHead;
    while( p != 0 ) {
        if( p->file != NULL ) {
            p->file->UnLoad( );
        }
    }
}

/*
================
MixerXAudio2::GetAttachedAudioFileCount
================
*/
U32 MixerXAudio2::GetAttachedAudioFileCount( void ) const {
    U32 attachedCount = 0;

    AudioFileMixerView *p = listHead;
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
MixerXAudio2::GetActiveAudioFileCount
================
*/
U32	MixerXAudio2::GetActiveAudioFileCount( void ) const {
    U32 activeCount = 0;

    AudioFileMixerView *p = listHead;
    while( p != NULL ) {
        if( p->file != NULL && p->file->IsPlaying( ) ) {
            ++activeCount;
        }
        p = p->next;
    }

    return activeCount;
}

/*
================
MixerXAudio2::Play
================
*/
void MixerXAudio2::Play( void ) {
    AudioFileMixerView *p = listHead;
    while( p != NULL ) {
        if( p->file != NULL && !p->file->IsPlaying( ) == false ) {
            p->file->Play( );
        }
        p = p->next;
    }
}

/*
================
MixerXAudio2::Pause
================
*/
void MixerXAudio2::Pause( void ) {
    AudioFileMixerView *p = listHead;
    while( p != NULL ) {
        if( ( p->file != NULL ) && ( p->file->IsPlaying( ) == true ) ) {
            p->file->Pause( );
        }
        p = p->next;
    }
}

/*
================
MixerXAudio2::Stop
================
*/
void MixerXAudio2::Stop( void ) {
    AudioFileMixerView *p = listHead;
    while( p != NULL ) {
        if( ( p->file != NULL ) && ( p->file->IsPlaying( ) == true ) ) {
            p->file->Stop( );
        }
        p = p->next;
    }
}

/*
================
MixerXAudio2::SetLoopingParams
================
*/
void MixerXAudio2::SetLoopingParams( U32 loopStart, U32 loopDuration, U32 loopCount ) {
    AudioFileMixerView *p = listHead;
    while( p != NULL ) {
        if( ( p->file != NULL ) ) {
            p->file->SetLoopingParams( loopStart, loopDuration, loopCount );
        }
        p = p->next;
    }
}

/*
================
MixerXAudio2::SetVolume
================
*/
void MixerXAudio2::SetVolume( F32 vol ) {
    AudioFileMixerView *p = listHead;
    while( p != NULL ) {
        if( ( p->file != NULL ) ) {
            p->file->SetVolume( vol );
        }
        p = p->next;
    }

    //submixVoice->SetVolume( ( vol * RT_VOLUME_SCALE_FACTOR ), XAUDIO2_COMMIT_NOW ); 
}

/*
================
MixerXAudio2::IsRunning
================
*/
bool MixerXAudio2::IsRunning( void ) const {
    return isRunning;
}

// TEMPORARY HELPER
void MixerXAudio2::SET_TEMP_MIXER_HELPER( void *p ) {
    tempHelper = reinterpret_cast<IXAudio2*>( p );
}


// ALT CREATE & DESTROY FOR TESTING
Mixer* CreateMixer( void *mem ) {
    MixerXAudio2 *temp = new(mem)MixerXAudio2;
    return reinterpret_cast<Mixer*>(temp);
}

void DestroyMixer(Mixer *p) {
    // destroy
    //delete p;
    reinterpret_cast<MixerXAudio2*>(p)->~MixerXAudio2();
}

U32 GetMixerMemoryRequirement( void ) {
    return sizeof(MixerXAudio2);
}
