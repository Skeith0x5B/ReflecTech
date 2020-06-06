/*
    ReflecTech
    -----------
    File        :   RtDynLibWin.cpp
    Author      :   Jamie Taylor
    Last Edit   :   04/09/12
    Desc        :   Represents a DLL/SO; can be used on its own or with the manager. This approach is OGRE inspired, an approach
                    of my own did ensure that DLLs/SOs were always tracked but it promoted coupling too much; also this approach allows for
                    both "static and dynamic allocation".

                    _H - using heap allocator
                    _DP - using dynamic pool allocator
                    _V - volatile; source code likely/probable to change
*/
#include "RtDynLibLin.h"


/**************************************************************************************************************************/
// Implementation
/**************************************************************************************************************************/
DynLib::DynLib(bool byMnger)
{
    internalHandle = 0;
    byManager = byMnger;
}
/**************************************************************************************************************************/

DynLib::~DynLib()
{
    // DLL/SO hasn't been freed explcitly (- free it now)?
    if(internalHandle)
    {
        std::cout << "Freeing..." << std::endl;
        // free it via the manager or internally - depends how it was created
        if(byManager)
            // manager.getInstance()...
            byManager = false; // temp...
        else
            Unload();
    }
}
/**************************************************************************************************************************/

void DynLib::Load(const I8 *name)
{
    // RTLD_LAZY - relocations done at an implementation/application specified time as opposed to being done on intial load
    // RTLD_GLOBAL - symbols made available for the relocation of any other object, objects load like this can also be searched by dlsym()
    internalHandle = dlopen(name, RTLD_LAZY | RTLD_GLOBAL);

    // check
    if(internalHandle == 0)
    {
        std::cout << "Error loading library: ";
        GetError();
    }
}
/**************************************************************************************************************************/

void DynLib::Unload()
{
    if(dlclose(internalHandle) != 0)
    {
        std::cout << "Error freeing library: ";
        GetError();
    }
    else
        internalHandle = 0;
}
/**************************************************************************************************************************/

void* DynLib::GetSymbol(const I8 *symbolName) const
{
    void *symbol = dlsym(internalHandle, symbolName);

    if(symbol == 0)
    {
        std::cout << "Error loading symbol: ";
        GetError();
    }

    return symbol;
}
/**************************************************************************************************************************/

handle DynLib::GetHandle() const
{
    return internalHandle;
}
/**************************************************************************************************************************/

void DynLib::GetError() const
{
    std::cout << dlerror() << std::endl;
}
/**************************************************************************************************************************/

bool DynLib::IsFromManager() const
{
    return byManager;
}
/**************************************************************************************************************************/
