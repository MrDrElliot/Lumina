#pragma once
#include "Core/Assertions/Assert.h"
#include "Platform/WindowsPlatform.h"
#include "rpmalloc.h"

static bool GIsMemorySystemInitialized = false;
static rpmalloc_config_t GrpmallocConfig;

#define DEFAULT_ALIGNMENT 8

//-------------------------------------------------------------------------
// Note: We dont globally overload the new or delete operators
//-------------------------------------------------------------------------
// Right now we use the default system allocators for new/delete calls and static allocations
// If in the future, we hit performance issues with allocation then we can consider globally overloading those operators
//-------------------------------------------------------------------------

class FMemory
{
public:

    FORCEINLINE static void MemsetZero( void* ptr, size_t size )
    {
        memset( ptr, 0, size );
    }
    
    template <typename T>
    FORCEINLINE static void MemsetZero( T* ptr )
    {
        memset( ptr, 0, sizeof( T ) );
    }

    FORCEINLINE static bool IsAligned(void const* p, size_t n)
    {
        return ( reinterpret_cast<uintptr_t>( p ) % n ) == 0;
    }

    template <typename T>
    FORCEINLINE static bool IsAligned(T const* p)
    {
        return ( reinterpret_cast<uintptr_t>( p ) % alignof( T ) ) == 0;
    }
    
    static void CustomAssert( char const* pMessage )
    {
        LOG_CRITICAL("Memory Error!: {0}", pMessage);
    }

    static void Initialize()
    {
        Assert(!GIsMemorySystemInitialized);
        memset(&GrpmallocConfig, 0, sizeof(rpmalloc_config_t));
        GrpmallocConfig.error_callback = &CustomAssert;

        rpmalloc_initialize_config(&GrpmallocConfig);

        GIsMemorySystemInitialized = true;
        
    }

    static void Shutdown()
    {
        GIsMemorySystemInitialized = false;
        rpmalloc_finalize();
    }
    
    void InitializeThreadHeap()
    {
        // Since our tasks are not bound to a specific thread and we may alloc on one and free on another. This prevents us from calling thread finalize when we shutdown a thread
        // as we can not guarantee that we have freed everything that may have been allocated from this thread.
        // This is not a problem since on application shutdown, we call rpmalloc_finalize, which will release the thread heaps
        rpmalloc_thread_initialize();
    }

    void ShutdownThreadHeap()
    {
        rpmalloc_thread_finalize(1);
    }

    static size_t GetTotalRequestedMemory()
    {
        rpmalloc_global_statistics_t stats;
        rpmalloc_global_statistics(&stats);
        return stats.mapped;
    }

    static size_t GetTotalAllocatedMemory()
    {
        rpmalloc_global_statistics_t stats;
        rpmalloc_global_statistics(&stats);
        return stats.mapped_total;
    }


    [[nodiscard]] static void* Malloc(size_t size, size_t alignment = DEFAULT_ALIGNMENT)
    {
        if (size == 0) return nullptr;
        void* pMemory = nullptr;
        pMemory = rpaligned_alloc(alignment, size);

        Assert(IsAligned(pMemory, alignment));
        return pMemory;
    }

    [[nodiscard]] static void* Realloc( void* pMemory, size_t newSize, size_t originalAlignment = DEFAULT_ALIGNMENT)
    {
        void* pReallocatedMemory = nullptr;

        pReallocatedMemory = rprealloc( pMemory, newSize );


        Assert(pReallocatedMemory != nullptr);
        return pReallocatedMemory;
    }

    static void Free( void*& pMemory )
    {
        rpfree((uint8_t*)pMemory);
        pMemory = nullptr;
    }

    template< typename T, typename ... ConstructorParams >
    [[nodiscard]] static FORCEINLINE T* New(ConstructorParams&&... params)
    {
        void* pMemory = Malloc( sizeof( T ), alignof( T ) );
        Assert(pMemory != nullptr);
        return new(pMemory) T( std::forward<ConstructorParams>(params )...);
    }

    template< typename T >
    static FORCEINLINE void Delete(T*& pType)
    {
        if ( pType != nullptr )
        {
            pType->~T();
            Free( (void*&) pType );
        }
    }

    template< typename T >
    FORCEINLINE void Free( T*& pType )
    {
        Free( (void*&) pType );
    }
    
};



