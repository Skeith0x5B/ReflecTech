/*
===============================================================================

    ReflecTech
    ==========
    File        :    RtBitmapFont.h
    Author      :    Jamie Taylor
    Last Edit   :    22/07/13
    Desc        :    Represents a bitmap font.

===============================================================================
*/


#ifndef RT_BITMAP_FONT_H
#define RT_BITMAP_FONT_H


#include "../../CoreSystems/RtTokenizer.h"
#include "../../PlatformIndependenceLayer/RtPlatform.h"
// temp
#include "../../CoreSystems/RtHeapAllocator.h"

// describes a character in a bitmap font
struct BitmapCharacter {
    BitmapCharacter( void ) : posX( 0 ), posY( 0 ), width( 0 ), height( 0 ), xOffset( 0 ), yOffset( 0 ), xAdvance( 0 ) { ; }

    U16 posX, posY;
    U16 width, height;
    I16 xOffset, yOffset;
    U16 xAdvance;
};

// describes the bitmap font overall
struct BitmapFont {
    BitmapFont( void ) : size( 0 ), lineHeight( 0 ), base( 0 ), textureWidth( 0 ), textureHeight( 0 ) { ; }

    // common/texture-wide properties
    U16 size;
    U16 lineHeight;
    U16 base;
    U16 textureWidth, textureHeight;

    // the individual characters, character values/ids will be used to index into this array
    // (will need to sub 1 from each?)
    BitmapCharacter characters[128];
};

// the default arial font used by the engine
const BitmapFont defaultEngineFont;

// strings been drawn by the engine will be described in one of these structures
struct StringDescription {
    StringDescription( void ) : font( const_cast<BitmapFont*>( &defaultEngineFont ) ),
        posX( 0 ), posY( 0 ), fontSize( 0 ) { fontColour[0] = fontColour[1] = fontColour[2] = fontColour[3] = 0; }

    BitmapFont * font;
    F32          fontColour[4];
    F32          posX, posY;
    F32          fontSize;
};

// move these functions to the filesystem/fileLoader utility class?
bool LoadBitmapFont( const I8 *filename, BitmapFont &font );
// clear the memory buffer and read the next token, calling this reduces the number of lines of code overall
void ClearAndRead( Tokenizer &tokenizer, I8 *tokenBuffer, I8 *delimiters, U32 delimiterCount );


#endif // RT_BITMAP_FONT_H
