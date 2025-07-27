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
        ~FCObjectAllocator()
        {
            Memory::Free(Buffer);
        }
        
        LUMINA_API CObjectBase* AllocateCObject(uint32 Size, uint32 Alignment);
        LUMINA_API void FreeCObject(CObjectBase* Ptr);

    private:

        uint8* Buffer = nullptr;
        SIZE_T Offset = 0;
        SIZE_T Max = 0;
        
    };
    
    extern LUMINA_API FCObjectAllocator GCObjectAllocator;

}
