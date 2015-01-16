/*
===============================================================================

	ReflecTech
	==========
	File		:	RtMixer.h
	Author		:	Jamie Taylor
	Last Edit	:	31/03/13
	Desc		:	A mixer is used to collectively manage and control a collection of wav file.

===============================================================================
*/


#ifndef RT_MIXER_H
#define RT_MIXER_H


#include "../CoreSystems/RtSinglyLinkedList_DP_V.h"
#include "RtWavFile.h"


/*
===============================================================================

Wave file class

===============================================================================
*/
class Mixer {
public:
					//Mixer( );
	virtual			~Mixer( void ) { };

	virtual bool	Start( void ) = 0;
	virtual void	Shutdown( void ) = 0;

	virtual void	AttachAudioFile( WavFile *audioFile ) = 0;
	virtual void	DetachAudioFile( WavFile *audioFile ) = 0;
	virtual void	DetachAllAudioFiles( void ) = 0;

	virtual U32		GetAttachedAudioFileCount( void ) const = 0;
	virtual U32		GetActiveAudioFileCount( void ) const = 0;

					// play, pause, stop all attached audio files
	virtual void	Play( void ) = 0;
	virtual void	Pause( void ) = 0;
	virtual void	Stop( void ) = 0;

					// set looping params for all sttached audio files
	virtual void	SetLoopingParams( U32 loopStart, U32 loopDuration, U32 loopCount ) = 0;
					// set volume for all attached audio files (range: 0.0f - 1.0f)
	virtual void	SetVolume( F32 vol ) = 0;

	virtual bool	IsRunning( void ) const = 0;

protected:
	bool			isRunning;
};


#endif // RT_MIXER_H