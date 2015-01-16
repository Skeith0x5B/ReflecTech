/*
	ReflecTech
	-----------
	File		:	MotherRng.cpp
	Author		:	Jamie Taylor
	Last Edit	:	27/05/12
	Desc		:	Random number generator based on the "mother" algorithm, designed by George Marsaglia. 
					This is Agner Fog's 64bit implementation made available under the GNU General Public License.
	-----------
	Agner Fog:

	This is a multiply-with-carry type of random number generator
	invented by George Marsaglia.  The algorithm is:             
	S = 2111111111*X[n-4] + 1492*X[n-3] + 1776*X[n-2] + 5115*X[n-1] + C
	X[n] = S modulo 2^32
	C = floor(S / 2^32)
	
	Further documentation:
	The file ran-instructions.pdf contains further documentation and 
	instructions.
	
	Copyright 1999-2008 by Agner Fog. 
	GNU General Public License http://www.gnu.org/licenses/gpl.html
*/
#include "RtMotherRng.h"


/**************************************************************************************************************************/
// Implementation
/**************************************************************************************************************************/
MotherRng::MotherRng(U32 seed)
{
	Init(seed);
}
/**************************************************************************************************************************/

// start-up
void MotherRng::Init(U32 seed)
{
	// make some random values and place them in the history buffer/array
	U32 i;
	for(i=0; i<5; ++i)
		history[i] = (seed *= (29943829 - 1));

	// randomize some more
	for(i=0; i<19; ++i)
		RandomBitPattern();
}
/**************************************************************************************************************************/

// get a random bit pattern
U32 MotherRng::RandomBitPattern()
{
	U64 sum = 2111111111UL * static_cast<U64>(history[3]) +
		static_cast<U64>(1492) * static_cast<U64>(history[2]) +
		static_cast<U64>(1776) * static_cast<U64>(history[1]) +
		static_cast<U64>(5115) * static_cast<U64>(history[0]) +
		static_cast<U64>(history[4]);

	// move the old values up one, making room for new carry and value
	history[3] = history[2]; 
	history[2] = history[1]; 
	history[1] = history[0];

	// get the new carry
	history[4] = static_cast<U32>(sum >> 32);

	// get the lower 32 bits of the sum - this is the newly generated value to return
	return (history[0] = static_cast<U32>(sum));
}
/**************************************************************************************************************************/

// get a random real number
F64 MotherRng::RandomReal()
{
	return static_cast<F64>((RandomBitPattern() * (1.0 / (65536.0*65536.0))));
}
/**************************************************************************************************************************/

// get a random integer within a specified range
U32 MotherRng::RandomRangedInt(U32 min, U32 max)
{
	// output random integer in the interval min <= x <= max
    // relative error on frequencies < 2^-32
	if(max <= min)
		return (max == min)? min : 0x80000000;

	// use multiply and shift approach
	U32 interval = max - min + 1; // length of interval
	U64 longRan = static_cast<U64>(RandomBitPattern() * interval); // random bits * interval
	U32 iran = static_cast<U32>(longRan >> 32); // longran / 2^32

	// convert back to signed and return result
	return static_cast<U32>(iran + min);
}
/**************************************************************************************************************************/
