#include "ObjectAllocator.h"

#include "ObjectBase.h"
#include "Memory/Memory.h"

namespace Lumina
{
    LUMINA_API FCObjectAllocator GCObjectAllocator;

    
    CObjectBase* FCObjectAllocator::AllocateCObject(int32 Size, int32 Alignment)
    {
        void* RawMemory = Memory::Malloc(Size + Alignment - 1);
        void* AlignedMemory = RawMemory;

        SIZE_T Space = Size + Alignment - 1;
        if (std::align(Alignment, Size, AlignedMemory, Space))
        {
            return static_cast<CObjectBase*>(AlignedMemory);
        }
        return nullptr;
    }

    void FCObjectAllocator::FreeCObject(CObjectBase* Ptr)
    {
        Assert(Ptr);
        Memory::Free(Ptr);
    }
}
