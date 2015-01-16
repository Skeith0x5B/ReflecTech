/*
===============================================================================

	ReflecTech
	==========
	File		:	RtDynamicLibraryWindows.h
	Author		:	Jamie Taylor
	Last Edit	:	14/07/13
	Desc		:	Represents a DLL/SO.

					_H - using heap allocator
					_DP - using dynamic pool allocator
					_V - volatile; source code likely/probable to change

===============================================================================
*/


#ifndef RT_DYNAMIC_LIBRARY_WINDOWS_H
#define RT_DYNAMIC_LIBRARY_WINDOWS_H


#include "../RtDynamicLibrary.h"


/*
===============================================================================

Dynamic Library class - Windows implementation

===============================================================================
*/
class DynamicLibrary {
public:
				  // let this default to false; the dllManager will set this as needed
				  DynamicLibrary( void );
				  ~DynamicLibrary( void );

	bool		  Load( const I8 *libraryName );
	void		  Unload( void );

				  // get a symbol from the library, null on failure
	void        * GetSymbol( const I8 *symbolName ) const; 

				  // get the handle for the library
	libraryHandle GetHandle( void ) const;
				  // get the last error that occured concerning this dynamic library
	void		  GetError( void ) const;

private:
	libraryHandle internalHandle;
};


#endif // RT_DYNAMIC_LIBRARY_WIN_H
