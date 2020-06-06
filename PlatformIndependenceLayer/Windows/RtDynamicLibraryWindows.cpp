/*
===============================================================================

    ReflecTech
    ==========
    File        :   RtDynamicLibraryWindows.h
    Author      :   Jamie Taylor
    Last Edit   :   29/06/13
    Desc        :   Represents a DLL/SO.

                    _H - using heap allocator
                    _DP - using dynamic pool allocator
                    _V - volatile; source code likely/probable to change

===============================================================================
*/


#include "RtDynamicLibraryWindows.h"


/*
================
DynamicLibrary::DynamicLibrary
================
*/
DynamicLibrary::DynamicLibrary( void ) {
    internalHandle = NULL;
}

/*
================
DynamicLibrary::~DynamicLibrary
================
*/
DynamicLibrary::~DynamicLibrary( void ) {
    // DLL/SO hasn't been freed explcitly (- free it now)?
    if( internalHandle != NULL ) {
        std::cout << "Freeing..." << std::endl;
        Unload( );
    }
}

/*
================
DynamicLibrary::Load
================
*/
bool DynamicLibrary::Load( const I8 *libraryName ) {
    internalHandle = LoadLibraryEx( reinterpret_cast<LPCSTR>( libraryName ), 0, LOAD_WITH_ALTERED_SEARCH_PATH );

    // check
    if( internalHandle == NULL ) {
        std::cout << "Error loading library: ";
        GetError( );
        return false;
    }

    // TODO: add to manager's list...
    // if can't register return false

    return true;
}

/*
================
DynamicLibrary::Unload
================
*/
void DynamicLibrary::Unload( void ) {
    if( FreeLibrary(internalHandle) == NULL ) {
        std::cout << "Error freeing library: ";
        GetError( );
    }
    else {
        internalHandle = NULL;
    }
    // TODO: remove from manager's list...
}

/*
================
DynamicLibrary::GetSymbol
================
*/
void* DynamicLibrary::GetSymbol(const I8 *symbolName) const {
    void *symbol = GetProcAddress( internalHandle, reinterpret_cast<LPCSTR>( symbolName ) );

    //check
    if( symbol == NULL ) {
        std::cout << "Error loading symbol: ";
        GetError( );
    }

    return symbol;
}

/*
================
DynamicLibrary::GetHandle
================
*/
libraryHandle DynamicLibrary::GetHandle() const {
    return internalHandle;
}

/*
================
DynamicLibrary::GetError
================
*/
void DynamicLibrary::GetError( void ) const {
    std::cout << GetLastError( ) << std::endl;
}
