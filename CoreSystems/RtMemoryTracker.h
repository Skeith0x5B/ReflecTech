/*
	Reflec-Tech
	-----------
	File		:	RtMemoryTracker.h
	Author		:	Jamie Taylor
	Last Edit	:	12/08/12
	Desc		:	Basic memory tracker, at the base level it tracks the memory consumption and peak memory consumption
					of each engine component peak memory usage for each engine component and/or each component of the application
					being developed with the engine.
					
					With RT_MEMORY_TRACKER enabled we can keep track of allocations made, bytes allocated and peak memory consumption.
					With RT_MEMORY_TRACKER_FULL enabled a hash table of AllocDesc structs will be maintained and flushed to a file
					when the tracker shuts down.
					
					Use an open-hasing/chaining approach when hash-tables are done as we can't accurately predict table load.

					LINK WITH SINGLETON?
*/   
#ifndef RT_MEMORY_TRACKER_H
#define RT_MEMORY_TRACKER_H


#include "../RtConfiguration.h"
#include "RtSinglyLinkedList_DP_V.h"
#include "RtSingleton.h"


#ifdef	RT_MEMORY_TRACKER


	/**************************************************************************************************************************/
	// Enums
	/**************************************************************************************************************************/	
	// potential sources/pools for memory allocations
	enum ALLOC_SOURCE
	{
		// Used as a 'default' of sorts
		ALLOC_SOURCE_NULL = -1,

		PRE_ALLOCD_RETAIL = 0,
		PRE_ALLOCD_DEVELOPMENT,
		DYNAMIC_RETAIL,
		DYNAMIC_DEVELOPMENT,

		// Used for array sizing
		ALLOC_SOURCE_COUNT,
	};
	/**************************************************************************************************************************/	
	  
	// engine/application component for which the memory is being allocated
	enum ALLOC_USAGE
	{
		// Used as a 'default' of sorts
		ALLOC_USAGE_NULL = -1,

		// Engine related
		ENGINE_GENERAL = 0,
		ENGINE_OS,
		ENGINE_SUBSYSTEM_RENDERING,
		ENGINE_SUBSYSTEM_AUDIO,
		ENGINE_SUBSYSTEM_IO,
		ENGINE_DEBUG,
		// ADD ADDITIONAL ENGINE OPTIONS HERE...

		// Application related - Application being built with the engine
		APPLICATION_GENERAL,
		APPLICATION_DEBUG, 
		// ADD ADDITIONAL APPLICATON OPTIONS HERE...

		// Used for array sizing
		ALLOC_USAGE_COUNT,
	};
	/**************************************************************************************************************************/	


	/**************************************************************************************************************************/
	// Definition
	/**************************************************************************************************************************/
	class MemoryTracker : public Singleton<MemoryTracker>
	{
	public:
		// ctor/dtor
		MemoryTracker();
		~MemoryTracker();

		// startup and shutdown
		void Startup();
		void Shutdown();
			
		// record/track allocations
		void RecordAlloc(void *p, const I8 *file, U32 line, size_t bytes, U8 alignment, ALLOC_SOURCE source, ALLOC_USAGE usage);
		void RecordDeAlloc(void *p);
			
		// get memory usage information for a particular system/component
		size_t GetMemoryUsage(ALLOC_SOURCE memPool, ALLOC_USAGE system);
		size_t GetPeakMemoryUsage(ALLOC_SOURCE memPool, ALLOC_USAGE system);

		// singleton releated code
		static MemoryTracker& GetSingletonReference();
		static MemoryTracker* GetSingletonPointer();
			
	private:
		U32 allocCount;
		size_t memoryUsage[ALLOC_SOURCE_COUNT][ALLOC_USAGE_COUNT];
		size_t peakMemoryUsage[ALLOC_SOURCE_COUNT][ALLOC_USAGE_COUNT];
			
		struct AllocDesc
		{
			AllocDesc() 
				{ p = 0; file = 0; line = 0; bytes = 0; alignment = 0; source = ALLOC_SOURCE_NULL; usage = ALLOC_USAGE_NULL; }

			AllocDesc(void *_p, const I8 *_file, U32 _line, size_t _bytes, U8 _alignment, ALLOC_SOURCE _source, ALLOC_USAGE _usage) 
				{ p = _p; file = _file; line = _line; bytes = _bytes; alignment = _alignment; source = _source; usage = _usage; }

			void *p;
			const I8 *file;
			U32 line;
			size_t bytes;
			U8 alignment;
			ALLOC_SOURCE source;
			ALLOC_USAGE usage;
		};

		// FUNCTION() - WRITE ALLOCATION RECORDS TO FILE - NEEDS (FILE SYSTEM / LOG MANAGER(specify log file)?)

		// --------------------------------------- TEMPORARY PLACE-HOLDER CODE ------------------------------------------------
		// HASH TABLE<AllocDesc> (when finished) - use a singly-linked list for now (only temporary)
		SinglyLinkedList<AllocDesc> allocList;
		// --------------------------------------- TEMPORARY PLACE-HOLDER CODE ------------------------------------------------
	};
	/**************************************************************************************************************************/


	template<> MemoryTracker *Singleton<MemoryTracker>::singletonInstance = 0;


#endif // RT_MEMORY_TRACKER


#endif // RT_MEMORY_TRACKER_H
