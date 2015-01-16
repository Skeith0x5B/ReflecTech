/*
===============================================================================

	ReflecTech
	==========
	File		:	RtBitmapFont.cpp
	Author		:	Jamie Taylor
	Last Edit	:	08/07/13
	Desc		:	Represents a bitmap font.

===============================================================================
*/


#include "RtBitmapFont.h"
// atoi & atof (write my own for the learning experience?)
#include <stdlib.h>


/*
================
LoadBitmapFont
================
*/
bool LoadBitmapFont( const char *filename, BitmapFont &font ) {
	HeapAllocator<void> heapAllctr;
	Tokenizer tokenizer;

	// load the file
	HANDLE fileHwnd = CreateFile( filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( fileHwnd == INVALID_HANDLE_VALUE ) {
		return false;
	}
	
	// read in the file
	LARGE_INTEGER fileSize;
	GetFileSizeEx( fileHwnd, &fileSize );

	I8 *fileBuffer = reinterpret_cast<I8*>( heapAllctr.Allocate( sizeof( I8 ) * fileSize.QuadPart ) );
	U32 bytesRead = 0;

	if( ReadFile( fileHwnd, reinterpret_cast<void*>( &fileBuffer[0] ), fileSize.QuadPart,
		          reinterpret_cast<LPDWORD>( &bytesRead ), NULL ) == 0 ) {
		CloseHandle( fileHwnd );
		return false;
	}

	// done reading from the file, close it
	if( CloseHandle( fileHwnd ) == 0 ) {
		return false;
	}

	// now parse the bitmap font related information and store it 
	// using the bitmap font structs described above

	// common info
	I8 tokenBuffer[64] = { 0 };
	I8 delimiters[2] = { ' ', '=' };
	tokenizer.SetBuffer( &fileBuffer[0], fileSize.QuadPart );

	// parse the common info we care about
	while( strcmp( tokenBuffer, "count" ) != 0 ) {
		ClearAndRead( tokenizer, tokenBuffer, delimiters, 2 );

		if( strcmp( tokenBuffer, "size" ) == 0 ) {
			ClearAndRead( tokenizer, tokenBuffer, delimiters, 2 );
			font.size = atoi( tokenBuffer );
		}

		if( strcmp( tokenBuffer, "lineHeight" ) == 0 ) {
			ClearAndRead( tokenizer, tokenBuffer, delimiters, 2 );
			font.lineHeight = atoi( tokenBuffer );
		}

		if( strcmp( tokenBuffer, "base" ) == 0 ) {
			ClearAndRead( tokenizer, tokenBuffer, delimiters, 2 );
			font.base = atoi( tokenBuffer );
		}

		if( strcmp( tokenBuffer, "scaleW" ) == 0 ) {
			ClearAndRead( tokenizer, tokenBuffer, delimiters, 2 );
			font.textureWidth = atoi( tokenBuffer );
		}

		if( strcmp( tokenBuffer, "scaleH" ) == 0 ) {
			ClearAndRead( tokenizer, tokenBuffer, delimiters, 2 );
			font.textureHeight = atoi( tokenBuffer );
		}
	} // while( )
	// how many characters are described in this file?
	ClearAndRead( tokenizer, tokenBuffer, delimiters, 2 );
	U32 charCount = atoi( tokenBuffer );

	// stop at kerning info
	U32 charID = 0;
	for( U32 i=0; i<charCount;  ) {
		tokenizer.GetNextToken( &tokenBuffer[0], delimiters, 2 );

		if( strcmp( tokenBuffer, "id" ) == 0 ) {
			ClearAndRead( tokenizer, tokenBuffer, delimiters, 2 );
			charID = atoi( tokenBuffer );
		}

		if( strcmp( tokenBuffer, "x" ) == 0 ) {
			ClearAndRead( tokenizer, tokenBuffer, delimiters, 2 );
			font.characters[charID].posX = atoi( tokenBuffer );
		}

		if( strcmp( tokenBuffer, "y" ) == 0 ) {
			ClearAndRead( tokenizer, tokenBuffer, delimiters, 2 );
			font.characters[charID].posY = atoi( tokenBuffer );
		}

		if( strcmp( tokenBuffer, "width" ) == 0 ) {
			ClearAndRead( tokenizer, tokenBuffer, delimiters, 2 );
			font.characters[charID].width = atoi( tokenBuffer );
		}

		if( strcmp( tokenBuffer, "height" ) == 0 ) {
			ClearAndRead( tokenizer, tokenBuffer, delimiters, 2 );
			font.characters[charID].height = atoi( tokenBuffer );
		}

		if( strcmp( tokenBuffer, "xoffset" ) == 0 ) {
			ClearAndRead( tokenizer, tokenBuffer, delimiters, 2 );
			font.characters[charID].xOffset = atoi( tokenBuffer );
		}

		if( strcmp( tokenBuffer, "yoffset" ) == 0 ) {
			ClearAndRead( tokenizer, tokenBuffer, delimiters, 2 );
			font.characters[charID].yOffset = atoi( tokenBuffer );
		}

		if( strcmp( tokenBuffer, "xadvance" ) == 0 ) {
			ClearAndRead( tokenizer, tokenBuffer, delimiters, 2 );
			font.characters[charID].xAdvance = atoi( tokenBuffer );
			++i;
		}

		memset( tokenBuffer, 0, 64 );		
	} // for( )

	heapAllctr.DeAllocate( reinterpret_cast<void*>( fileBuffer ) );
	fileBuffer = NULL;

	// done
	return true;
} 

/*
================
ClearAndRead
================
*/
void ClearAndRead( Tokenizer &tokenizer, char *tokenBuffer, char *delimiters, U32 delimiterCount ) {
	memset( tokenBuffer, 0, 64 );
	tokenizer.GetNextToken( &tokenBuffer[0], delimiters, 2 );
}