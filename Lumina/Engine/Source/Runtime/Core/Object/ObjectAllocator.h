#pragma once

#include "Memory/Memory.h"
#include "Module/API.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class CObjectBase;
}

namespace Lumina
{
    class FCObjectAllocator
    {
    public:

        FCObjectAllocator();
        ~FCObjectAllocator();
        
        LUMINA_API CObjectBase* AllocateCObject(uint32 Size, uint32 Alignment);
        LUMINA_API void FreeCObject(CObjectBase* Ptr);

    private:
        
    };
    
    extern LUMINA_API FCObjectAllocator GCObjectAllocator;

}
