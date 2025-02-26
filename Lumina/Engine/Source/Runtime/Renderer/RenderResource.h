#pragma once

#include "Containers/String.h"
#include "Memory/RefCounted.h"

namespace Lumina
{
    class IRenderContext;
}

/**
 * Base class for all render resources. Lifetime is managed internally with atomic intrusive
 * reference counting.
 */
namespace Lumina
{
    class IRenderResource
    {
    public:

        IRenderResource() = default;
        virtual ~IRenderResource() { }
        
        

        //--------------------------------------------------------------------------
        // Reference counting. TODO: batch deletion.
        
        uint32 AddRef() const
        {
            int32 NewValue = eastl::atomic_add_fetch_explicit(&RefCount, 1, eastl::memory_order_relaxed);
            Assert(NewValue > 0);
            return uint32(NewValue);
        }
        
        uint32 Release() const
        {
            int32 NewValue = eastl::atomic_sub_fetch_explicit(&RefCount, 1, eastl::memory_order_release);
            
            if (NewValue == 0)
            {
                // Make sure all previous operations (memory writes) are visible before deleting
                eastl::atomic_thread_fence(eastl::memory_order_acquire);
                IRenderResource* MutableThis = const_cast<IRenderResource*>(this);
                FMemory::Delete(MutableThis);
            }
            
            Assert(NewValue >= 0);
            return uint32(NewValue);
        }
        
        uint32 GetRefCount() const { return RefCount.load(eastl::memory_order_relaxed); }

        
    private:
        
        mutable eastl::atomic<int> RefCount = 0;
        
        
    };

}