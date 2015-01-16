/*
	Reflec-Tech
	-----------
	File		:	RtMemoryTracker.cpp
	Author		:	Jamie Taylor
	Last Edit	:	12/08/12
	Desc		:	Basic memory tracker, at the base level it tracks the memory consumption and peak memory consumption
					of each engine component peak memory usage for each engine component and/or each component of the application
					being developed with the engine.
					
					With RT_MEMORY_TRACKER enabled we can keep track of allocations made, bytes allocated and peak memory consumption.
					With RT_MEMORY_TRACKER_FULL enabled a hash table of AllocDesc structs will be maintained and flushed to a file
					when the tracker shuts down.
					
					Use an open-hasing/chaining approach when hash-tables are done as we can't accurately predict table load.
*/ 
#include "RtMemoryTracker.h"


#ifdef RT_MEMORY_TRACKER


	/**************************************************************************************************************************/
	// Implementation
	/**************************************************************************************************************************/
	MemoryTracker::MemoryTracker()
	{
		// ...
	}
	/**************************************************************************************************************************/

	MemoryTracker::~MemoryTracker()
	{
		// ...
	}
	/**************************************************************************************************************************/

	void MemoryTracker::Startup()
	{
		allocCount = 0;
			
		for(U32 i=0; i<ALLOC_SOURCE_COUNT; ++i)
			for(U32 j=0; j<ALLOC_USAGE_COUNT; ++j)
				memoryUsage[i][j] = peakMemoryUsage[i][j] = 0;
	}
	/**************************************************************************************************************************/

	void MemoryTracker::Shutdown()
	{
		std::cout << "-------------------------------------------------------" << std::endl; 
		if(allocCount != 0 || !allocList.IsEmpty())
		{
			// --------------------------------------- TEMPORARY PLACE-HOLDER CODE ------------------------------------------------
			// the only entries in the allocation list/record at this point will detail those allocations that were never matched with 
			// a corresponding de-allocation, so print the allocDesc(s) to a file and notify user

			std::cout << "Memory Leak(s) Detected: " << allocCount << std::endl;
			std::cout << "-------------------------------------------------------" << std::endl;

			SinglyLinkedList<AllocDesc>::ConstIterator itr(allocList.CBegin());
			while((&(*itr) != 0x0))
			{
				std::cout << "file : " << itr->file << std::endl;
				std::cout << "line : " << itr->line << std::endl;
				std::cout << "bytes: " << itr->bytes << std::endl;
				std::cout << "-------------------------------------------------------\n" << std::endl;
				++itr;
			}
			// write allocation info to file
		}
		else
		{
			std::cout << "No Memory Leak(s) Detected"	<< std::endl; 	
		}
		std::cout << "-------------------------------------------------------" << std::endl;
		// --------------------------------------- TEMPORARY PLACE-HOLDER CODE ------------------------------------------------
	}
	/**************************************************************************************************************************/

	// ENGINE_ALL, APPLICATION_ALL or ALL should be passed in via dest, since these two are primarily used to help us navigate the arrays
	void MemoryTracker::RecordAlloc(void *p,const I8 *file, U32 line, size_t bytes, U8 alignment, ALLOC_SOURCE source, ALLOC_USAGE usage)
	{
		++allocCount;
		memoryUsage[source][usage] += bytes;
			
		if(bytes > peakMemoryUsage[source][usage])
			peakMemoryUsage[source][usage] = bytes;

		AllocDesc thisAlloc(p, file, line, bytes, alignment, source, usage);
		// --------------------------------------- TEMPORARY PLACE-HOLDER CODE ------------------------------------------------
		allocList.PushFront(thisAlloc);
		// --------------------------------------- TEMPORARY PLACE-HOLDER CODE ------------------------------------------------
	}
	/**************************************************************************************************************************/

	// ENGINE_ALL, APPLICATION_ALL or ALL should be passed in via dest, since these two are primarily used to help us navigate the arrays
	void MemoryTracker::RecordDeAlloc(void *p)
	{
	    --allocCount;
	
		// --------------------------------------- TEMPORARY PLACE-HOLDER CODE ------------------------------------------------
		// with the hash table this'll be replaced with a simple look up
		SinglyLinkedList<AllocDesc>::Iterator itr(allocList.Begin());
		while(itr->p != p)
			++itr;
		memoryUsage[itr->source][itr->usage] -= (*itr).bytes; // hashTable[hashThis(p)].bytes;
			
		// remove entry from allocation list/record
		// this will equate to marking the hash entry as 'DELETED'
		allocList.Remove(itr);	
		// --------------------------------------- TEMPORARY PLACE-HOLDER CODE ------------------------------------------------
	}
	/**************************************************************************************************************************/

	size_t MemoryTracker::GetMemoryUsage(ALLOC_SOURCE memPool, ALLOC_USAGE system)
	{
		return memoryUsage[memPool][system];
	}
	/**************************************************************************************************************************/

	size_t MemoryTracker::GetPeakMemoryUsage(ALLOC_SOURCE memPool, ALLOC_USAGE system)
	{
		return peakMemoryUsage[memPool][system];
	}
	/**************************************************************************************************************************/

	MemoryTracker& MemoryTracker::GetSingletonReference()
	{ 
		return *singletonInstance; 
	}
	/**************************************************************************************************************************/

	MemoryTracker* MemoryTracker::GetSingletonPointer()
	{
		return singletonInstance; 
	}
	/**************************************************************************************************************************/

		
#endif // RT_MEMORY_TRACKER