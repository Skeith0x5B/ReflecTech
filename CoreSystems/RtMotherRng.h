/*
    ReflecTech
    -----------
    File        :    RtMotherRng.h
    Author      :    Jamie Taylor
    Last Edit   :    27/05/12
    Desc        :    Random number generator based on the "mother" algorithm, designed by George Marsaglia. 
                     This is Agner Fog's 64bit implementation made available under the GNU license.
    -----------
    Agner Fog:

    "This is a multiply-with-carry type of random number generator
    invented by George Marsaglia.  The algorithm is:             
    S = 2111111111*X[n-4] + 1492*X[n-3] + 1776*X[n-2] + 5115*X[n-1] + C
    X[n] = S modulo 2^32
    C = floor(S / 2^32)

    Copyright 1999-2008 by Agner Fog. 
    GNU General Public License http://www.gnu.org/licenses/gpl.html"
*/
#ifndef RT_MOTHER_RNG_H
#define RT_MOTHER_RNG_H


#include "../RtCommonHeaders.h"


/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
class MotherRng
{
public:
    // Ctor
    MotherRng(U32 seed);

    // Initialise
    void Init(U32 seed);

    // Get a random bit pattern
    U32 RandomBitPattern();

    // Get a random floating point / double value
    F64 RandomReal();

    // Get a random int within a given range
    U32 RandomRangedInt(U32 min, U32 max);

private:
    U32 history[5]; // holds previous values and carries
};
/**************************************************************************************************************************/


#endif // RT_MOTHER_RNG_H
