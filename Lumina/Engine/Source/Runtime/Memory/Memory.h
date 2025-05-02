#pragma once

#include <EASTL/type_traits.h>
#include <rpmalloc.h>

#include "Core/Assertions/Assert.h"
#include "Module/API.h"
#include "Platform/WindowsPlatform.h"
#include <Core/Assertions/Assert.h>

static bool GIsMemorySystemInitialized = false;
static rpmalloc_config_t GrpmallocConfig;

#define DEFAULT_ALIGNMENT 8


class LUMINA_API FMemory
{
public:

    template <typename T>
    static constexpr typename eastl::remove_reference<T>::type&& Move(T&& x) noexcept
    {
        return static_cast<typename eastl::remove_reference<T>::type&&>(x);
    }

    template <typename T>
    static constexpr typename eastl::enable_if<std::is_move_constructible_v<T> && !eastl::is_trivially_copyable_v<T>, eastl::remove_reference_t<T>&&>::type
    MoveIfAble(T&& x) noexcept
    {
        return static_cast<typename eastl::remove_reference<T>::type&&>(x);
    }

    template <typename T>
    static constexpr eastl::enable_if_t<!std::is_move_constructible_v<T> || eastl::is_trivially_copyable_v<T>, T&>
    MoveIfAble(T& x) noexcept
    {
        return x;
    }

    FORCEINLINE static void MemsetZero(void* ptr, size_t size)
    {
        memset(ptr, 0, size);
    }
    
    template <typename T>
    FORCEINLINE static void MemsetZero(T* ptr)
    {
        memset(ptr, 0, sizeof(T));
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
        std::cerr << pMessage << "\n";
    }

    static void Initialize()
    {
        if (!GIsMemorySystemInitialized)
        {
            memset(&GrpmallocConfig, 0, sizeof(rpmalloc_config_t));
            GrpmallocConfig.error_callback = &CustomAssert;

            rpmalloc_initialize_config(&GrpmallocConfig);

            GIsMemorySystemInitialized = true;

            std::cout << "Memory System Initialized \n";
        }
    }

    static void Shutdown()
    {
        std::cout << "Memory System Shutdown Reamining Allocations: " << "\n";
        GIsMemorySystemInitialized = false;
        rpmalloc_finalize();
    }
    
    static void InitializeThreadHeap()
    {
        // Since our tasks are not bound to a specific thread and we may alloc on one and free on another. This prevents us from calling thread finalize when we shutdown a thread
        // as we can not guarantee that we have freed everything that may have been allocated from this thread.
        // This is not a problem since on application shutdown, we call rpmalloc_finalize, which will release the thread heaps
        rpmalloc_thread_initialize();
    }

    static void ShutdownThreadHeap()
    {
        rpmalloc_thread_finalize(1);
    }

    NODISCARD static size_t GetTotalRequestedMemory()
    {
        rpmalloc_global_statistics_t stats;
        rpmalloc_global_statistics(&stats);
        return stats.mapped;
    }

    NODISCARD static size_t GetTotalAllocatedMemory()
    {
        rpmalloc_global_statistics_t stats;
        rpmalloc_global_statistics(&stats);
        return stats.mapped_total;
    }


    NODISCARD static void* Malloc(size_t size, size_t alignment = DEFAULT_ALIGNMENT)
    {
        Assert(size != 0);
        
        if (UNLIKELY(!GIsMemorySystemInitialized))
        {
            FMemory::Initialize();
        }
        
        void* pMemory = nullptr;
        pMemory = rpaligned_alloc(alignment, size);
        Assert(IsAligned(pMemory, alignment));

        return pMemory;
    }

    NODISCARD static void Memcpy(void* Destination, void* Source, uint64 SrcSize)
    {
        memcpy(Destination, Source, SrcSize);
        Assert(Destination != nullptr);
    }
    

    NODISCARD static void* Realloc(void* pMemory, size_t newSize, size_t originalAlignment = DEFAULT_ALIGNMENT)
    {
        void* pReallocatedMemory = nullptr;

        pReallocatedMemory = rprealloc(pMemory, newSize);


        Assert(pReallocatedMemory != nullptr);
        return pReallocatedMemory;
    }

    static void Free(void*& pMemory)
    {
        rpfree((uint8_t*)pMemory);
        pMemory = nullptr;
    }

    template< typename T, typename ... ConstructorParams >
    requires std::is_constructible_v<T, ConstructorParams...> && (!eastl::is_array_v<T>)
    NODISCARD static FORCEINLINE T* New(ConstructorParams&&... params)
    {
        void* pMemory = Malloc(sizeof(T), alignof(T));
        return new(pMemory) T(eastl::forward<ConstructorParams>(params)...);
    }

    template<typename T>
    static FORCEINLINE void Delete(T*& pType)
    {
        if (pType != nullptr)
        {
            pType->~T();
            Free( (void*&) pType );
        }
    }

    template< typename T >
    static FORCEINLINE void Free(T*& pType)
    {
        Free( (void*&) pType );
    }
    


    // Basic single object allocation
    void* operator new(std::size_t size)
    {
        return FMemory::Malloc(size);
    }

    void operator delete(void* ptr) noexcept
    {
        FMemory::Free(ptr);
    }

    // Array allocation
    void* operator new[](std::size_t size)
    {
        return FMemory::Malloc(size);
    }

    void operator delete[](void* ptr) noexcept
    {
        FMemory::Free(ptr);
    }

#define DECLARE_MODULE_ALLOCATOR_OVERRIDES() \
    void* operator new(std::size_t size) { return FMemory::Malloc(size); } \
    void operator delete(void* ptr) noexcept { FMemory::Free(ptr); } \
    void* operator new[](std::size_t size) { return FMemory::Malloc(size); } \
    void operator delete[](void* ptr) noexcept { FMemory::Free(ptr); } \
    void* operator new(std::size_t size, std::align_val_t align) { return FMemory::Malloc(size, static_cast<size_t>(align)); } \
    void* operator new[](std::size_t size, std::align_val_t align) { return FMemory::Malloc(size, static_cast<size_t>(align)); } \
    void operator delete(void* ptr, std::align_val_t) noexcept { FMemory::Free(ptr); } \
    void operator delete[](void* ptr, std::align_val_t) noexcept { FMemory::Free(ptr); } \
    void* operator new(std::size_t size, const std::nothrow_t&) noexcept { return FMemory::Malloc(size); } \
    void* operator new[](std::size_t size, const std::nothrow_t&) noexcept { return FMemory::Malloc(size); } \
    void operator delete(void* ptr, const std::nothrow_t&) noexcept { FMemory::Free(ptr); } \
    void operator delete[](void* ptr, const std::nothrow_t&) noexcept { FMemory::Free(ptr); } \
    void operator delete(void* ptr, std::size_t) noexcept { FMemory::Free(ptr); } \
    void operator delete[](void* ptr, std::size_t) noexcept { FMemory::Free(ptr); } \
    void operator delete(void* ptr, std::size_t, std::align_val_t) noexcept { FMemory::Free(ptr); } \
    void operator delete[](void* ptr, std::size_t, std::align_val_t) noexcept { FMemory::Free(ptr); } \

};



