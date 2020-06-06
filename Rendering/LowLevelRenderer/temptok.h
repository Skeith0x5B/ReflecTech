/*
===============================================================================

    ReflecTech0
    ==========
    File        :    RtTokenizer.h
    Author      :    Jamie Taylor
    Last Edit   :    20/04/13
    Desc        :    A basic tokenizer, primarily written to parse .obj files.
                     A tokenizer takes a string and breaks it down into smaller strings called tokens,
                     various delimiters can be used to help identify tokens.

                     NOTE: Trying to include the tokenizer in the main project is causing linker issues, this
                           is a temporary work around so I can keep working on other things until I find
                           a solution to the linker issue.

===============================================================================
*/


#ifndef RT_TOKENIZER_H
#define RT_TOKENIZER_H


#include "../../PlatformIndependenceLayer/RtPlatform.h"


/*
===============================================================================

Tokenizer class

===============================================================================
*/
class Tokenizer {
public:
            Tokenizer( void );
            ~Tokenizer( void );

    void    SetBuffer( I8 *buffer_, U32 bufferSize_ );
    void    ResetBuffer( void );
            // stop pointing to the buffer
    void    ReleaseBuffer( void );

    bool    GetNextToken( I8 *buffer_, I8 *delimiters, U32 delimiterCount );
    bool    ClearAndRead( I8 *buffer_, U32 bufferSize, I8 *delimiters, U32 delimiterCount );

private:
            // checks wether *c is an accepted ascii character
    bool    IsValid( I8 *c ) const;
    bool    IsDelimiter( I8 *c, I8 *delimiters, U32 delimiterCount );

            // can be from a file or any other source
    I8    * buffer;
    U32     bufferSize;
    U32     currentTokenStartIndex;
    U32     currentTokenEndIndex;
};


#endif // RT_TOKENIZER_H
