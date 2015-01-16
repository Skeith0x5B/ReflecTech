/*
===============================================================================

	ReflecTech
	==========
	File		:	RtDynamicLibraryManager.h
	Author		:	Jamie Taylor
	Last Edit	:	14/07/13
	Desc		:	Dynamic library manager, libraries will automatically register themselves
					with the manager on creation and unregister themselves on destruction. The 
					manager keeps track of all actively loaded dynamic libraries.

					- don't see a need for this, simply declaring an instance?
					template<> DynamicLibraryManager *Singleton<DynamicLibraryManager>::singletonInstance = NULL;

===============================================================================
*/


#ifndef RT_DYNAMIC_LIBRARY_MANAGER_H
#define RT_DYNAMIC_LIBRARY_MANAGER_H


#include "RtDynamicLibrary.h"
#include "../CoreSystems/RtSingleton.h"
#include "../CoreSystems/RtHeapAllocator.h"
#include "../CoreSystems/RtSinglyLinkedList_DP_V.h"


/*
===============================================================================

Dynamic Library Manager

===============================================================================
*/
class DynamicLibraryManager : public Singleton<DynamicLibraryManager> {
public:
										 // the library class will register itself on loading and unregister itself on unloading
	friend class						 DynamicLibrary;

										 // ctor/dtor
										 DynamicLibraryManager( void );
										 ~DynamicLibraryManager( void );

										 // startup and shutdown
	void								 Startup( void );
	void								 Shutdown( void );

										 // null on failure
	void								 LoadLibrary( const I8 *libraryToLoad, DynamicLibrary &library );
									     // false on failure
	void								 UnloadLibrary( DynamicLibrary &libraryToUnload );

										 // singleton related
	static DynamicLibraryManager	   & GetSingletonReference( void );
	static DynamicLibraryManager       * GetSingletonPointer( void );

private:
										 // holds info about the library
										 struct DynamicLibraryDesc {
															 DynamicLibraryDesc( I8 *_name = "\0", DynamicLibrary *_dynLib = NULL, bool isDynamicallyAllocated_ = false ) { 
																libraryName = _name; library = _dynLib; isDynamicallyAllocated = isDynamicallyAllocated_;
															 }

											I8	  		   * libraryName;
											DynamicLibrary * library;
											bool			 isDynamicallyAllocated;
										 };
											
	SinglyLinkedList<DynamicLibraryDesc> libraryList;
										 // if the manager is unable to find the requested library it will create it using this allocator
	HeapAllocator<DynamicLibrary>		 memAllctr;

										 // libraries will call these on loading/unloading to add/remove themselves from the list
	void								 RegisterLibrary( void );
	void								 UnRegisterLibrary( void );

										 DynamicLibraryManager( const DynamicLibraryManager &ref ) { /* do nothing - forbidden op */ }
	DynamicLibraryManager			   & operator=( const DynamicLibraryManager &rhs ) { /* do nothing - forbidden op */ }
};


template<> DynamicLibraryManager *Singleton<DynamicLibraryManager>::singletonInstance = NULL;


#endif // RT_DYNAMIC_LIBRARY_MANAGER_H