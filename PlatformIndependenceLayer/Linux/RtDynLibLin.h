/*
	Reflec-Tech
	-----------
	File		:	RtDynLibLin.h
	Author		:	Jamie Taylor
	Last Edit	:	11/09/12
	Desc		:	Represents a DLL/SO; can be used on its own or with the manager. This approach is OGRE inspired, an approach
				of my own did ensure that DLLs/SOs were always tracked but it promoted coupling too much; also this approach allows for
				both "static and dynamic allocation".

				_H - using heap allocator
				_DP - using dynamic pool allocator
				_V - volatile; source code likely/probable to change
*/
#ifndef RT_DYN_LIB_LIN_H
#define RT_DYN_LIB_LIN_H


#include "../RtDynLib.h"



/**************************************************************************************************************************/
// Definition
/**************************************************************************************************************************/
class DynLib
{
public:
	// let this default to false; the dllManager will set this as needed
	DynLib(bool byMnger = false);
	~DynLib();

	void Load(const I8 *name);
	void Unload();

	// get a symbol from the library, null on failure
	void* GetSymbol(const I8 *symbolName) const; 

	// get the handle for the library
	handle GetHandle() const;
	// get the last error that occured concerning this dynamic library
	void GetError() const;

	// did/is the dllManager allocate/make/load/is tracking this DLL
	bool IsFromManager() const;

private:
	handle internalHandle;
	// user doesn't/shouldn't mess with this value
	bool byManager;
};
/**************************************************************************************************************************/


#endif // RT_DYN_LIB_LIN_H
