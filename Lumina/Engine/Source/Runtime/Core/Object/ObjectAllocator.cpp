#include "ObjectAllocator.h"

#include "ObjectBase.h"
#include "Memory/Memory.h"

namespace Lumina
{
    LUMINA_API FCObjectAllocator GCObjectAllocator;

    
    CObjectBase* FCObjectAllocator::AllocateCObject(int32 Size, int32 Alignment)
    {
        void* RawMemory = Memory::Malloc(Size + Alignment - 1);

        uintptr_t RawPtr = reinterpret_cast<uintptr_t>(RawMemory);
        uintptr_t AlignedPtr = (RawPtr + Alignment - 1) & ~(Alignment - 1);

        return reinterpret_cast<CObjectBase*>(AlignedPtr);
    }

    void FCObjectAllocator::FreeCObject(CObjectBase* Ptr)
    {
        Assert(Ptr);
        Memory::Free(Ptr);
    }
}
