/*
===============================================================================

    ReflecTech
    ==========
    File        :   RtWavFile.h
    Author      :   Jamie Taylor
    Last Edit   :   25/03/13
    Desc        :   A basic static wav file class. Adopting a DOOM 3 style syntax.

===============================================================================
*/


#ifndef RT_WAV_FILE_H
#define RT_WAV_FILE_H


#include "../PlatformIndependenceLayer/RtPlatform.h"


#define RT_VOLUME_SCALE_FACTOR 1.75f


/*
===============================================================================

Wave file class

===============================================================================
*/
class WavFile {
public:
                    //WaveFile( void );
    virtual         ~WavFile( void ) { };

    virtual bool    Load( const I8 *fileName ) = 0;
    virtual void    UnLoad( void ) = 0;

    virtual void    Play( void ) = 0;
    virtual void    Pause( void ) = 0;
    virtual void    Stop( void ) = 0;

                    // loopStart and loopDuration expect a number of seconds
    virtual void    SetLoopingParams( U32 loopStart, U32 loopDuration, U32 loopCount, bool immediateEffect = true ) = 0;
                    
                    // within the range 0.0f - 1.0f
    virtual F32     GetVolume( void ) const = 0;
    virtual void    SetVolume( F32 vol ) = 0;

    virtual bool    IsLoaded( void ) const = 0;
    virtual bool    IsPlaying( void ) const = 0;

protected:
    bool            isLoaded;
    bool            isPlaying;
    U8*             audioBuffer;
};


#endif // RT_WAV_FILE_H
