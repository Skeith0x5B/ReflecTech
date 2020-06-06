/*
===============================================================================

    ReflecTech
    ==========
    File        :    RtDynamicLibraryManager.h
    Author      :    Jamie Taylor
    Last Edit   :    14/07/13
    Desc        :    Dynamic library manager, libraries will automatically register themselves
                     with the manager on creation and unregister themselves on destruction. The 
                     manager keeps track of all actively loaded dynamic libraries.

===============================================================================
*/


#include "RtDynamicLibraryManager.h"


/*
================
DynamicLibraryManager::DynamicLibraryManager
================
*/
DynamicLibraryManager::DynamicLibraryManager( void ) {
    // ...
}

/*
================
DynamicLibraryManager::~DynamicLibraryManager
================
*/
DynamicLibraryManager::~DynamicLibraryManager( void ) {
    // ...
}

/*
================
DynamicLibraryManager::Startup
================
*/
void DynamicLibraryManager::Startup( void ) {
    // ...
}

/*
================
DynamicLibraryManager::Shutdown
================
*/
void DynamicLibraryManager::Shutdown( void ) {
    // clean-up internal dynLib list/record
    if( libraryList.IsEmpty( ) == false ) {
        SinglyLinkedList<DynamicLibraryDesc>::Iterator itr( libraryList.Begin( ) );
        while( ( &(*itr) != NULL ) ) {
            itr->library->Unload( );

            if( itr->isDynamicallyAllocated == true ) {
                memAllctr.Destruct( itr->library );
                memAllctr.DeAllocate( itr->library );
            }

            libraryList.Remove(itr);
            itr = libraryList.Begin();
        }
    }
}

/*
================
DynamicLibraryManager::LoadLibrary
================
*/
void DynamicLibraryManager::LoadLibrary( const I8 *libraryToLoad, DynamicLibrary &library ) {
    // search the list for the library
    SinglyLinkedList<DynamicLibraryDesc>::ConstIterator itr( libraryList.CBegin( ) );
    // while node been pointed to is not NULL and the name doesn't match
    while( ( &(*itr) != NULL ) && ( strcmp( itr->libraryName, libraryToLoad ) != 0 ) ) {
            ++itr;
    }

    // have we found it?
    if( ( &(*itr) != NULL ) && ( strcmp( itr->libraryName, libraryToLoad ) == 0 ) ) {
        library = *itr->library;
    } 

    // didn't find it, create a new library instance and load it
    DynamicLibraryDesc tempDesc( const_cast<I8*>( libraryToLoad ), &library, false );

    // ...

    // add it to the list
    libraryList.PushFront( tempDesc );
}

/*
================
DynamicLibraryManager::UnloadLibrary
================
*/
void DynamicLibraryManager::UnloadLibrary( DynamicLibrary &libraryToUnLoad  ) {
    // search the list for the library
    libraryHandle tempHandle = libraryToUnLoad.GetHandle( );
    SinglyLinkedList<DynamicLibraryDesc>::Iterator itr( libraryList.Begin( ) );
    while( ( &(*itr) != NULL ) && ( itr->library->GetHandle( ) != tempHandle ) ) {
            ++itr;
    }

    // found it? remove it
    if( ( &(*itr) != NULL ) && ( itr->library->GetHandle( ) == tempHandle ) ) {
        itr->library->Unload( );

        if( itr->isDynamicallyAllocated == true ) {
            memAllctr.Destruct( itr->library );
            memAllctr.DeAllocate( itr->library );
        }

        libraryList.Remove( itr );
    }
}

/*
================
DynamicLibraryManager::Register
================
*/
void DynamicLibraryManager::RegisterLibrary( void ) {
    //// check it's not already in the library
    //// --------------------------------------- TEMPORARY PLACE-HOLDER CODE ------------------------------------------------
    //SinglyLinkedList<DynLibDesc>::ConstIterator itr(dynLibList.CBegin());
    //while((&(*itr) != 0x0) && (itr->nameOfDynLib != dllName))
    //        ++itr;

    //// it's already present - stop now and return
    //if((&(*itr) != 0x0) && (itr->nameOfDynLib == dllName))
    //    return;

    //// add it to the list/record
    //DynLibDesc tempDesc(dllName, const_cast<DynLib*>(dllToAdd));
    //dynLibList.PushFront(tempDesc);
    //// --------------------------------------- TEMPORARY PLACE-HOLDER CODE ------------------------------------------------
}

/*
================
DynamicLibraryManager::UnRegister
================
*/
void DynamicLibraryManager::UnRegisterLibrary( void ) {

}


/*
================
DynamicLibraryManager::GetSingletonReference
================
*/
DynamicLibraryManager& DynamicLibraryManager::GetSingletonReference( void ) {
    return *singletonInstance;
}

/*
================
DynamicLibraryManager::GetSingletonPointer
================
*/
DynamicLibraryManager* DynamicLibraryManager::GetSingletonPointer( void ) {
    return singletonInstance;
}
