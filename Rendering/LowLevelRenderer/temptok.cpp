/*
===============================================================================

	ReflecTech
	==========
	File		:	RtTokenizer.cpp
	Author		:	Jamie Taylor
	Last Edit	:	20/04/13
	Desc		:	A basic tokenizer, primarily written to parse .obj files.
					A tokenizer takes a string and breaks it down U32o smaller strings called tokens,
					various delimiters can be used to help identify tokens.

===============================================================================
*/


#include "temptok.h"


/*
================
Tokenizer::Tokenizer
================
*/
Tokenizer::Tokenizer( void ) {
	// set initial safe state of 0/NULL
	ReleaseBuffer( );
}

/*
================
Tokenizer::Tokenizer
================
*/
Tokenizer::~Tokenizer( void ) {
	ReleaseBuffer( );
}

/*
================
Tokenizer::Tokenizer
================
*/
void Tokenizer::SetBuffer( I8 *buffer_, U32 bufferSize_ ) {
	buffer = buffer_;
	bufferSize = bufferSize_;
}

/*
================
Tokenizer::Tokenizer
================
*/
void Tokenizer::ResetBuffer( void ) {
	// prepare to start reading from the beginning
	currentTokenStartIndex = 0;
	currentTokenEndIndex = 0;
}

/*
================
Tokenizer::Tokenizer
================
*/
void Tokenizer::ReleaseBuffer( void ) {
	// stop pointing to the buffer, doesn't actually free it,
	// that's the job of the calling code
	buffer = NULL;
	bufferSize = 0;
	currentTokenStartIndex = 0;
	currentTokenEndIndex = 0;
}

/*
================
Tokenizer::Tokenizer
================
*/
bool Tokenizer::GetNextToken( I8 *buffer_, I8 *delimiters, U32 delimiterCount ) {
	// will return true while there are still tokens to be read
	I8 *outgoingBuffer = reinterpret_cast<I8*>( buffer_ );
	//I8 *internalBuffer  = reinterpret_cast<I8*>( buffer );

	// have we reached the end of the buffer?
	if( currentTokenEndIndex == bufferSize ) {
		return false;
	}

	// don't start on a delimiter
	while( IsDelimiter( &buffer[currentTokenStartIndex], delimiters, delimiterCount ) == true ) {
		++currentTokenStartIndex;
	}

	currentTokenEndIndex = currentTokenStartIndex;

	// parse a token (stop when a delimiter is found)
	U32 i = 0;
	while( (currentTokenEndIndex != ( bufferSize ) ) && ( IsValid( &buffer[currentTokenEndIndex] ) == true ) &&
		   ( IsDelimiter( &buffer[currentTokenEndIndex], delimiters, delimiterCount ) == false )  ) {
		outgoingBuffer[i] = buffer[currentTokenEndIndex];
		++currentTokenEndIndex;
		++i;
	}

	// prepare for next token
	currentTokenStartIndex = ( currentTokenEndIndex + 1 );

	return true;
}

/*
================
Tokenizer::ClearAndRead
================
*/
 bool Tokenizer::ClearAndRead( I8 *buffer_, U32 bufferSize, I8 *delimiters, U32 delimiterCount ) {
     memset( buffer_, 0x00, bufferSize );
     bool readResult = GetNextToken( buffer_, delimiters, delimiterCount );
     return readResult;
 }

/*
================
Tokenizer::Tokenizer
================
*/
bool Tokenizer::IsValid( I8 *c ) const {
	// is *c a valid ascii character?
	U32 i = static_cast<U32>( *c );
    if( ( i > 32 ) && ( i < 127 ) ) {
		return true;
	}
	return false;
}

/*
================
Tokenizer::Tokenizer
================
*/
bool Tokenizer::IsDelimiter( I8 *c, I8 *delimiters, U32 delimiterCount ) {
	for( U32 i=0; i<delimiterCount; ++i ) {
		if ( *c == delimiters[i] ) {
			return true;
		}
	}
	return false;
}
