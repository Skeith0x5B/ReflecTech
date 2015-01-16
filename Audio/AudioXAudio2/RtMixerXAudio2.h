/*
===============================================================================

	ReflecTech
	==========
	File		:	RtMixer.h
	Author		:	Jamie Taylor
	Last Edit	:	31/03/13
	Desc		:	An xAudio2 powered mixer class implementation.

===============================================================================
*/


#ifndef RT_MIXER_XAUDIO2_H
#define RT_MIXER_XAUDIO2_H


#include "../../PlatformIndependenceLayer/RtPlatform.h"
#include "../../CoreSystems/RtMemoryCommon.h"

// needed to test factory functions
#include "../../CoreSystems/RtHeapAllocator.h"

// memory corruption caused by allctr->contruct ("XAUDIO2 glitch at sample xyz")
//#include "../../CoreSystems/RtSinglyLinkedList_DP_V.h"

#include "../RtMixer.h"
#include "RtWavFileXAudio2.h"
#include <XAudio2.h>


/*
===============================================================================

XAudio2 powered mixer class

===============================================================================
*/
class MixerXAudio2 : public Mixer {
public:
											   MixerXAudio2( void );
											   ~MixerXAudio2( void );

	bool									   Start( void );
	void									   Shutdown( void );

	void									   AttachAudioFile( WavFile *audioFile );
	void									   DetachAudioFile( WavFile *audioFile );
	void									   DetachAllAudioFiles( void );

	U32										   GetAttachedAudioFileCount( void ) const;
	U32										   GetActiveAudioFileCount( void ) const;

											   // play, pause, stop all attached audio files
	void									   Play( void );
	void									   Pause( void );
	void									   Stop( void );

											   // set looping params for all sttached audio files
	void									   SetLoopingParams( U32 loopStart, U32 loopDuration, U32 loopCount );
											   // set volume for all attached audio files (range: 0.0f - 1.0f)
	void									   SetVolume( F32 vol );

	bool									   IsRunning( void ) const;

											   // XAudio2 helper
	void									   SET_TEMP_MIXER_HELPER( void *p );

private:
											   struct AudioFileMixerView {
											       WavFileXAudio2     *file;
											       AudioFileMixerView *next;
											   };

	AudioFileMixerView						 * listHead;
	DynamicPoolAllocator<AudioFileMixerView> * memAllctr;

	// TEMPORARY HELPER
	IXAudio2								 * tempHelper;

	IXAudio2SubmixVoice						 * submixVoice;	
};


// ALT CREATE & DESTROY FOR TESTING
extern "C" Mixer * CreateMixer( void *mem );
extern "C" void	   DestroyMixer( Mixer *p );
extern "C" U32	   GetMixerMemoryRequirement( void );


#endif // RT_MIXER_XAUDIO2_H