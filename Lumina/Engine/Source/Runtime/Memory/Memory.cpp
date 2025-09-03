#include "Memory.h"

namespace Lumina
{
    void Memory::Initialize()
    {
        if (!GIsMemorySystemInitialized)
        {
            Memzero(&GrpmallocConfig, sizeof(rpmalloc_config_t));
            GrpmallocConfig.error_callback = &CustomAssert;

            rpmalloc_initialize_config(&GrpmallocConfig);

            GIsMemorySystemInitialized = true;
            std::cout << "[Lumina] - Memory System Initialized\n";
        }
    }
    
    void Memory::Shutdown()
    {
        rpmalloc_global_statistics_t stats;
        rpmalloc_global_statistics(&stats);
        
        std::cout << "[Lumina] - Memory System Shutdown with " << stats.mapped << " bytes.\n";

        GIsMemorySystemInitialized = false;
        rpmalloc_finalize();
        GIsMemorySystemShutdown = true;
    }

    void Memory::InitializeThreadHeap()
    {
        // Since our tasks are not bound to a specific thread and we may alloc on one and free on another. This prevents us from calling thread finalize when we shutdown a thread
        // as we can not guarantee that we have freed everything that may have been allocated from this thread.
        // This is not a problem since on application shutdown, we call rpmalloc_finalize, which will release the thread heaps
        rpmalloc_thread_initialize();
    }

    SIZE_T Memory::GetActualAlignment(size_t size, size_t alignment)
    {
        // If alignment is 0 (DEFAULT_ALIGNMENT), pick based on size:
        // 8-byte alignment for small blocks (<16), otherwise 16-byte
        if (alignment == DEFAULT_ALIGNMENT)
        {
            return (size < 16) ? 8 : 16;
        }

        // Ensure minimum alignment rules
        SIZE_T defaultAlignment = (size < 16) ? 8 : 16;
        SIZE_T Align = (alignment < defaultAlignment) ? defaultAlignment : (alignment);

        return Align;
    }

    void Memory::Memcpy(void* Destination, void* Source, uint64 SrcSize)
    {
        memcpy(Destination, Source, SrcSize);
        Assert(Destination != nullptr)
    }

    void Memory::Memcpy(void* Destination, const void* Source, uint64 SrcSize)
    {
        memcpy(Destination, Source, SrcSize);
        Assert(Destination != nullptr)
    }

    void* Memory::Malloc(size_t size, size_t alignment)
    {
#if LUMINA_RPMALLOC
        
        if(!GIsMemorySystemInitialized)
        {
            Initialize();
        }
        
        if (size == 0)
        {
            return nullptr;
        }

        SIZE_T ActualAlignment = GetActualAlignment(size, alignment);
        void* pMemory = rpaligned_alloc(ActualAlignment, size);
        
        Assert(IsAligned(pMemory, ActualAlignment))

        LUMINA_PROFILE_ALLOC(pMemory, size);
#else
        SIZE_T ActualAlignment = GetActualAlignment(size, alignment);
        void* pMemory = _aligned_malloc(size, ActualAlignment);
        Assert(IsAligned(pMemory, ActualAlignment))
        LUMINA_PROFILE_ALLOC(pMemory, size);
#endif

        return pMemory;
    }

    void* Memory::Realloc(void* Memory, size_t NewSize, size_t OriginalAlignment)
    {
        SIZE_T ActualAlignment = GetActualAlignment(NewSize, OriginalAlignment);
        
#if LUMINA_RPMALLOC
        void* pReallocatedMemory = rpaligned_realloc(Memory, ActualAlignment, NewSize, 0, 0);
        Assert(pReallocatedMemory != nullptr)

#else
        void* pReallocatedMemory = _aligned_realloc(Memory, NewSize, ActualAlignment);
        Assert(pReallocatedMemory != nullptr)
#endif
        
        return pReallocatedMemory;
        
    }

    void Memory::Free(void*& Memory)
    {
#if LUMINA_RPMALLOC
        LUMINA_PROFILE_FREE(Memory);
        rpfree(Memory);
        Memory = nullptr;
#else
        LUMINA_PROFILE_FREE(Memory);
        _aligned_free(Memory);
        Memory = nullptr;
#endif
    }
}
