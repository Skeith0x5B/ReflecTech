/*
===============================================================================

	ReflecTech
	==========
	File		:	RtWavFileXAudio2.cpp
	Author		:	Jamie Taylor
	Last Edit	:	25/03/13
	Desc		:	An XAudio2 implementation of the basic static wave file class. Adopting a DOOM 3 style syntax.

===============================================================================
*/


#include "RtWavFileXAudio2.h"
#include "RtAudioDeviceXAudio2.h"


/*
================
WavFileXAudio2::WavFileXAudio2
================
*/
WavFileXAudio2::WavFileXAudio2( void ) {
	isLoaded  = false;
	isPlaying = false;

	audioFile   = NULL;
	audioBuffer = NULL;
	sourceVoice = NULL;

	// set member structs mem to 0
	//ZeroMem( &audioFileBuffer, sizeof( XAUDIO2_BUFFER ) );
	//ZeroMem( &audioFileFormat, sizeof( WAVEFORMATEXTENSIBLE ) );

	ZeroMemory( &audioFileBuffer, sizeof( XAUDIO2_BUFFER ) );
	ZeroMemory( &audioFileFormat, sizeof( WAVEFORMATEXTENSIBLE ) );
	ZeroMemory( &emitter, sizeof( X3DAUDIO_EMITTER ) );

	// register with manager here
    AudioDeviceXAudio2::GetSingletonReference( ).RegisterWavFile( reinterpret_cast<WavFile*>( this ) );
}

/*
================
WaveFileXAudio2::~WaveFileXAudio2
================
*/
WavFileXAudio2::~WavFileXAudio2( void ) {
	// unregister with manager here
    AudioDeviceXAudio2::GetSingletonReference( ).UnRegisterWavFile( reinterpret_cast<WavFile*>( this ) );

	// then unload
	if( isLoaded == true ) {
		UnLoad( );
	}
}

/*
================
WaveFileXAudio2::Load
================
*/
bool WavFileXAudio2::Load( const I8 *fileName ) {
	// put the filename into a 
	const I8 *fileNameCopy = fileName;
	I8 fileNameArray[256] = { 0 };
	U32 i = 0;
	while( *fileNameCopy ) {
		fileNameArray[i++] = *fileNameCopy++;
	}
	fileNameArray[i] = '\0';
	
	audioFile = CreateFile( fileNameArray, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ); // use FILE_FLAG_NO_BUFFERING for async streaming
	if ( audioFile == INVALID_HANDLE_VALUE ) {
		return false;
	}

	// read in the file, starting with the RIFF chunk
	U32 chunkPosition  = 0;
	U32 chunkSize	   = 0;
	U32 fileType	   = 0;

	// read the 'RIFF' chunk
	FindChunk( FOURCC_ID_RIFF, chunkSize, chunkPosition );
	ReadChunk( &fileType, chunkSize, chunkPosition );

	// read the 'fmt' section of the 'RIFF' chunk
	FindChunk( FOURCC_ID_FMT, chunkSize, chunkPosition );
	ReadChunk( &audioFileFormat.Format, chunkSize, chunkPosition );

	// only supporting Wave files at present
	if( fileType != FOURCC_ID_WAVE ) {
		UnLoad( );
		return false;
	}

	// read the 'data' section of the 'RIFF chunk, this contains our audio data
	FindChunk( FOURCC_ID_DATA, chunkSize, chunkPosition );
	audioBuffer = reinterpret_cast<U8*>( allocator.Allocate( ( sizeof( U8 ) * chunkSize ) ) );
	ReadChunk( audioBuffer, chunkSize, chunkPosition );

	// populate XAUDIO2_BUFFER
	// size of the audio buffer in bytes
	audioFileBuffer.AudioBytes = chunkSize;
	// buffer containing audio data
	audioFileBuffer.pAudioData = const_cast<U8*>( reinterpret_cast<U8*>( audioBuffer ) );
	// tell the source voice not to expect any data after this buffer
	audioFileBuffer.Flags = XAUDIO2_END_OF_STREAM;

	// setup the file's source voice, the files representation in XAudio2
	//AudioManagerXAudio2 *audioEngine = (AudioManagerXAudio2*)AudioManager::GetSingletonPointer( );
	AudioDeviceXAudio2 *amxa = (AudioDeviceXAudio2*)AudioDeviceXAudio2::GetSingletonPointer( );
	IXAudio2 *xAudio2Engine = amxa->GetXAudioEngine( );
	if( FAILED( xAudio2Engine->CreateSourceVoice( &sourceVoice, &audioFileFormat.Format, 0, 
													 XAUDIO2_MAX_FREQ_RATIO, this, NULL, NULL ) ) ) {
		UnLoad( );
		return false;
	}

	// setup the audio file's 3d audio properties
	emitter.Position			= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    emitter.OrientFront			= D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
    emitter.OrientTop			= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	emitter.ChannelCount		= 1; 
	emitter.CurveDistanceScaler = 2.0f;

	isLoaded = true;
	return true;
} // WaveFileXAudio2::Load

/*
================
WavFileXAudio2::UnLoad
================
*/
void WavFileXAudio2::UnLoad( void ) {
	if( isPlaying == true ) {
		Stop( );
	}

	// the source voice may also be destroyed by the audio manager, depending on the shutdown order
	// either ordering (manager->wav or wav->manager) is safe
	if( sourceVoice != NULL ) {
		sourceVoice->DestroyVoice( );
	}

	if( audioBuffer != NULL ) {
		allocator.DeAllocate( audioBuffer );
		audioBuffer = NULL;
	}

	if( audioFile != NULL ) {
		if ( CloseHandle( audioFile ) == 0) {
			audioFile = NULL;
		}
	}

	isLoaded = false;
}

/*
================
WavFileXAudio2::Play
================
*/
void WavFileXAudio2::Play( void ) {
	if( isPlaying == true ) {
		return;
	}

	HRESULT hr = 0;
	hr = sourceVoice->SubmitSourceBuffer( &audioFileBuffer, NULL );
	hr = sourceVoice->Start( 0, XAUDIO2_COMMIT_NOW );
	isPlaying = true;
}

/*
================
WavFileXAudio2::Pause
================
*/
void WavFileXAudio2::Pause( void ) {
	sourceVoice->Stop( );
	isPlaying = false;
}

/*
================
WavFileXAudio2::Stop
================
*/
void WavFileXAudio2::Stop( void ) {
	sourceVoice->Stop( );
	sourceVoice->FlushSourceBuffers( );
	isPlaying = false;
}

/*
================
WavFileXAudio2::SetLoopingParams

If immediate effect is set to true then the file will be restarted with this change in parameters.
If false then the changes will take effect on the next call to Play( )
================
*/
void WavFileXAudio2::SetLoopingParams( U32 loopStart, U32 loopDuration, U32 loopCount, bool immediateEffect ) {
	if( loopStart < 0 ) {
		loopStart = 0;
	}

	if( loopDuration < 0 ) {
		loopDuration = 0;
	}

	if( loopCount > XAUDIO2_MAX_LOOP_COUNT ) {
		loopCount = XAUDIO2_MAX_LOOP_COUNT;
	}

	if( (isPlaying == true ) && ( immediateEffect == true ) ) {
		Stop( );
	}

	audioFileBuffer.LoopBegin = ( loopStart * audioFileFormat.Format.nSamplesPerSec );
	audioFileBuffer.LoopLength = ( loopDuration * audioFileFormat.Format.nSamplesPerSec );
	audioFileBuffer.LoopCount = loopCount;

	if( immediateEffect == true ) {
		Play( );
	}
}

/*
================
WavFileXAudio2::GetVolume
================
*/
F32 WavFileXAudio2::GetVolume( void ) const {
	float vol = 0.0f;
	sourceVoice->GetVolume( &vol );

	vol /= RT_VOLUME_SCALE_FACTOR;
	return vol;
}

/*
================
WavFileXAudio2::SetVolume
================
*/
void WavFileXAudio2::SetVolume( F32 vol ) {
	if( vol < 0.0f ) {
		vol = 0.0f;
	}
	
	if( vol > 1.0f ) {
		vol = 1.0f;
	}

	sourceVoice->SetVolume( ( vol * RT_VOLUME_SCALE_FACTOR ), XAUDIO2_COMMIT_NOW );  
}

/*
================
WavFileXAudio2::IsLoaded
================
*/
bool WavFileXAudio2::IsLoaded( void ) const {
	return isLoaded;
}

/*
================
WavFileXAudio2::IsPlaying
================
*/
bool WavFileXAudio2::IsPlaying( void ) const {
	return isPlaying;
}

/*
================
WavFileXAudio2::IsPlaying

Implementation specific helper, gets XAudio2 related format information
================
*/
WAVEFORMATEXTENSIBLE* WavFileXAudio2::GetFormatData( void ) const {
	return const_cast<WAVEFORMATEXTENSIBLE*>( &audioFileFormat );
}

/*
================
WavFileXAudio2::IsPlaying

Implementation specific helper, gets XAudio2 related format information
================
*/
X3DAUDIO_EMITTER* WavFileXAudio2::Get3dEmitter( void ) const {
    return const_cast<X3DAUDIO_EMITTER*>( &emitter );
}

/*
================
WavFileXAudio2::IsPlaying

Implementation specific helper, gets XAudio2 related format information
================
*/
IXAudio2SourceVoice* WavFileXAudio2::GetSourceVoice( void ) const {
    return sourceVoice;
}

/*
================
WavFileXAudio2::FindChunk

true = chunk found, false = chunk not found
================
*/
bool WavFileXAudio2::FindChunk( U32 fourcc, U32 &chunkSize, U32 &chunkPosition ) {
	// set file pointer to the beginning of the file for safety
	if( SetFilePointer( audioFile, 0, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) {
		return false;
	}

	// find the chunk
	U32 chunkType	  = 0;
	U32 chunkDataSize = 0;
	U32 RIFFDataSize  = 0;
	U32 fileType	  = 0;
	U32 offset		  = 0;

	U32 bytesRead	= 0;
	U32 loopControl = 1;
	while( loopControl == 1 ) {
		// read the chunks format
		if( ReadFile( audioFile, &chunkType, sizeof( U32 ), reinterpret_cast<LPDWORD>( &bytesRead ), NULL ) == 0 ) {
			loopControl = HRESULT_FROM_WIN32( GetLastError( ) );
		}

		// read the chunks size
		if( ReadFile( audioFile, &chunkDataSize, sizeof( unsigned int ), reinterpret_cast<LPDWORD>( &bytesRead ), NULL ) == 0 ) {
			loopControl = HRESULT_FROM_WIN32( GetLastError( ) );
		}

		// act based on chunk type
		// act based on chunk type
		switch( chunkType ) {
		// RIFF chunk, is this a WAVE or xWMA file? what size is the RIFF chunk? 
		// (the RIFF chunk contains all the other chunks, it effectively is the audio file)
		case FOURCC_ID_RIFF:
			RIFFDataSize = chunkDataSize;
			chunkDataSize = 4; // ...
			if( ReadFile( audioFile, &fileType, sizeof( U32 ), reinterpret_cast<LPDWORD>( &bytesRead ), NULL ) == 0 ) {
				loopControl = HRESULT_FROM_WIN32( GetLastError( ) );
			}
			break;

		// skip over the chunk
		default:
			// set file pointer
			if( SetFilePointer( audioFile, chunkDataSize, 0, FILE_CURRENT ) == INVALID_SET_FILE_POINTER ) {
				loopControl = HRESULT_FROM_WIN32( GetLastError( ) );
			}
			break;
		} // switch( chunkType )

		// increment the offset
		offset += ( sizeof( unsigned int ) * 2 );

		// have we found the chunk?
		if( chunkType == fourcc ) {
			chunkSize = chunkDataSize;
			chunkPosition = offset;
			return true;
		}

		// add the size of what we've just read to the offset and keep looking (skip over this chunk), it's not the one we're looking for
		offset += chunkDataSize;

		// if we've read all or more bytes than exist in the RIFF chunk's data section then we're done / something's gone wrong / the chunk searched for isn't present
		if( bytesRead >= RIFFDataSize ) {
			return false;
		}
	} // while( 1 )

	// if the chunk has been found this point will not be hit
	return false;
} // WaveFileXAudio2::FindChunk( )

/*
================
WavFileXAudio2::ReadChunk

returns the number of bytes read, 0 indicates error
================
*/
U32	WavFileXAudio2::ReadChunk( void *buffer, U32 bytesToRead, U32 bufferOffset ) {
	// set file pointer
	if( SetFilePointer( audioFile, bufferOffset, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) {
		return 0;
	}

	// read
	// using this in asynchronous file IO is potentially unsafe: http://msdn.microsoft.com/en-gb/library/windows/desktop/aa365467(v=vs.85).aspx
	U32 bytesRead = 0;
	if( ReadFile( audioFile, buffer, bytesToRead, reinterpret_cast<LPDWORD>( &bytesRead ), NULL ) == 0 ) {
		return 0;
	}

	return bytesRead;
}

/*
================
WavFileXAudio2::OnStreamEnd

XAudio2 callback functions, here to end of file
================
*/
void WavFileXAudio2::OnStreamEnd( void ) { 
	Stop( ); 
}

/*
================
WavFileXAudio2::OnVoiceProcessingPassEnd
================
*/
void WavFileXAudio2::OnVoiceProcessingPassEnd( void ) {
	// stub
}

/*
================
WavFileXAudio2::OnVoiceProcessingPassStart
================
*/
void WavFileXAudio2::OnVoiceProcessingPassStart( U32 SamplesRequired ) {
	// stub
}

/*
================
WavFileXAudio2::OnBufferEnd
================
*/
void WavFileXAudio2::OnBufferEnd( void * pBufferContext ) { 
	// stub
}

/*
================
WavFileXAudio2::OnBufferStart
================
*/
void WavFileXAudio2::OnBufferStart( void * pBufferContext ) {
	// stub
}

/*
================
WavFileXAudio2::OnLoopEnd
================
*/
void WavFileXAudio2::OnLoopEnd( void * pBufferContext ) {
	// stub
}

/*
================
WavFileXAudio2::OnVoiceError
================
*/
void WavFileXAudio2::OnVoiceError( void * pBufferContext, HRESULT Error ) {
	// stub
}


// ALT CREATE & DESTROY FOR TESTING
WavFile* CreateWavFile( void *mem ) {
	WavFileXAudio2 *temp = new( mem )WavFileXAudio2;
	return reinterpret_cast<WavFile*>( temp );
}

void DestroyWavFile( WavFile *p ) {
	// destroy
	//delete p;
	reinterpret_cast<WavFileXAudio2*>( p )->~WavFileXAudio2( );
}

U32 GetWavFileMemoryRequirement( void ) {
	return sizeof( WavFileXAudio2 );
}
