#pragma once

#include <rpmalloc.h>
#include <EASTL/type_traits.h>
#include "Core/Assertions/Assert.h"
#include "Core/Math/Math.h"
#include "Module/API.h"
#include "Platform/Platform.h"
#include "tracy/TracyC.h"


#define LUMINA_PROFILE_ALLOC(p, size) TracyCAllocS(p, size, 12)
#define LUMINA_PROFILE_FREE(p) TracyCFreeS(p, 12)

inline bool GIsMemorySystemInitialized = false;
inline rpmalloc_config_t GrpmallocConfig;

constexpr SIZE_T DEFAULT_ALIGNMENT = 0;
constexpr SIZE_T MIN_ALIGNMENT = 8;

namespace Lumina::Memory
{
    template <typename T>
    constexpr typename eastl::remove_reference<T>::type&& Move(T&& x) noexcept
    {
        return static_cast<typename eastl::remove_reference<T>::type&&>(std::forward<T>(x));
    }

    template <typename T>
    constexpr auto MoveIfAble(T&& x) noexcept -> decltype(auto)
    requires (std::is_move_constructible_v<std::remove_reference_t<T>> && !eastl::is_trivially_copyable_v<std::remove_reference_t<T>>)
    {
        return static_cast<std::remove_reference_t<T>&&>(std::forward<T>(x));
    }

    template <typename T>
    constexpr auto MoveIfAble(T& x) noexcept -> decltype(auto)
    requires (!std::is_move_constructible_v<T> || eastl::is_trivially_copyable_v<T>)
    {
        return x;
    }

    LUMINA_API inline void Memzero(void* ptr, size_t size)
    {
        memset(ptr, 0, size);
    }
    
    template <typename T>
    inline void Memzero(T* ptr)
    {
        memset(ptr, 0, sizeof(T));
    }

    LUMINA_API inline bool IsAligned(void const* ptr, size_t n)
    {
        return (reinterpret_cast<uintptr_t>(ptr) % n) == 0;
    }

    template <typename T>
    inline bool IsAligned(T const* p)
    {
        return (reinterpret_cast<uintptr_t>(p) % alignof( T )) == 0;
    }

    LUMINA_API inline void CustomAssert(const char* pMessage)
    {
        const char* IgnorePrefix = "Memory leak detected";
        if (std::strncmp(pMessage, IgnorePrefix, std::strlen(IgnorePrefix)) == 0)
        {
            return;
        }

        std::cout << pMessage << "\n";
    }

    LUMINA_API inline void Initialize()
    {
        if (!GIsMemorySystemInitialized)
        {
            memset(&GrpmallocConfig, 0, sizeof(rpmalloc_config_t));
            GrpmallocConfig.error_callback = &CustomAssert;

            rpmalloc_initialize_config(&GrpmallocConfig);

            GIsMemorySystemInitialized = true;
            std::cout << "[Lumina] - Memory System Initialized\n";
        }
    }

    LUMINA_API inline void Shutdown()
    {
        GIsMemorySystemInitialized = false;
        rpmalloc_finalize();
    }

    LUMINA_API inline void InitializeThreadHeap()
    {
        // Since our tasks are not bound to a specific thread and we may alloc on one and free on another. This prevents us from calling thread finalize when we shutdown a thread
        // as we can not guarantee that we have freed everything that may have been allocated from this thread.
        // This is not a problem since on application shutdown, we call rpmalloc_finalize, which will release the thread heaps
        rpmalloc_thread_initialize();
    }

    LUMINA_API inline bool IsThreadHeapInitialized()
    {
        return rpmalloc_is_thread_initialized();
    }

    LUMINA_API inline void ShutdownThreadHeap()
    {
        rpmalloc_thread_finalize(1);
    }

    LUMINA_API NODISCARD inline size_t GetCurrentMappedMemory()
    {
        rpmalloc_global_statistics_t stats;
        rpmalloc_global_statistics(&stats);
        return stats.mapped;
    }

    LUMINA_API NODISCARD inline size_t GetPeakMappedMemory()
    {
        rpmalloc_global_statistics_t stats;
        rpmalloc_global_statistics(&stats);
        return stats.mapped_peak;
    }

    LUMINA_API NODISCARD inline size_t GetCachedMemory()
    {
        rpmalloc_global_statistics_t stats;
        rpmalloc_global_statistics(&stats);
        return stats.cached;
    }

    LUMINA_API NODISCARD inline size_t GetCurrentHugeAllocMemory()
    {
        rpmalloc_global_statistics_t stats;
        rpmalloc_global_statistics(&stats);
        return stats.huge_alloc;
    }

    LUMINA_API NODISCARD inline size_t GetPeakHugeAllocMemory()
    {
        rpmalloc_global_statistics_t stats;
        rpmalloc_global_statistics(&stats);
        return stats.huge_alloc_peak;
    }

    LUMINA_API NODISCARD inline size_t GetTotalMappedMemory()
    {
        rpmalloc_global_statistics_t stats;
        rpmalloc_global_statistics(&stats);
        return stats.mapped_total;
    }

    LUMINA_API NODISCARD inline size_t GetTotalUnmappedMemory()
    {
        rpmalloc_global_statistics_t stats;
        rpmalloc_global_statistics(&stats);
        return stats.unmapped_total;
    }

    LUMINA_API inline SIZE_T GetActualAlignment(size_t size, size_t alignment)
    {
        // If alignment is 0 (DEFAULT_ALIGNMENT), pick based on size:
        // 8-byte alignment for small blocks (<16), otherwise 16-byte
        if (alignment == DEFAULT_ALIGNMENT)
        {
            return (size < 16) ? 8 : 16;
        }

        // Ensure minimum alignment rules
        SIZE_T defaultAlignment = (size < 16) ? 8 : 16;
        SIZE_T Align = (alignment < defaultAlignment) ? defaultAlignment : static_cast<SIZE_T>(alignment);

        return Lumina::Math::Max<SIZE_T>(Align, alignment);
    }

    LUMINA_API NODISCARD inline void* Malloc(size_t size, size_t alignment = DEFAULT_ALIGNMENT)
    {
        Assert(size != 0)

        if (UNLIKELY(!GIsMemorySystemInitialized))
        {
            Memory::Initialize();
        }

        if (UNLIKELY(!IsThreadHeapInitialized()))
        {
            InitializeThreadHeap();
        }

        SIZE_T ActualAlignment = GetActualAlignment(size, alignment);
        void* pMemory = rpaligned_alloc(ActualAlignment, size);

        Assert(IsAligned(pMemory, ActualAlignment))

        LUMINA_PROFILE_ALLOC(pMemory, size);
        return pMemory;
    }

    LUMINA_API NODISCARD inline void Memcpy(void* Destination, void* Source, uint64 SrcSize)
    {
        memcpy(Destination, Source, SrcSize);
        Assert(Destination != nullptr)
    }
    
    LUMINA_API NODISCARD inline void Memcpy(void* Destination, const void* Source, uint64 SrcSize)
    {
        memcpy(Destination, Source, SrcSize);
        Assert(Destination != nullptr)
    }

    LUMINA_API NODISCARD inline void* Realloc(void* pMemory, size_t newSize, size_t originalAlignment = DEFAULT_ALIGNMENT)
    {
        void* pReallocatedMemory = rprealloc(pMemory, newSize);
        Assert(pReallocatedMemory != nullptr)
        return pReallocatedMemory;
    }

    LUMINA_API inline void Free(void*& Memory)
    {
        LUMINA_PROFILE_FREE(Memory);
        rpfree(Memory);
        Memory = nullptr;
    }

    template< typename T, typename ... ConstructorParams >
    requires std::is_constructible_v<T, ConstructorParams...> && (!eastl::is_array_v<T>)
    NODISCARD T* New(ConstructorParams&&... params)
    {
        void* Memory = Malloc(sizeof(T), alignof(T));
        // std::forward is used instead of eastl::forward to suppress static analysis warnings
        // (e.g. cppcoreguidelines-missing-std-forward). EASTL is still preferred in most cases.
        return new(Memory) T(std::forward<ConstructorParams>(params)...);
    }

    template<typename T>
    void Delete(T* Type)
    {
        if (Type != nullptr)
        {
            Type->~T();
            Free((void*&)Type);
        }
    }

    template< typename T >
    void Free(T*& Type)
    {
        Free((void*&)Type);
    }
    
}



#define DECLARE_MODULE_ALLOCATOR_OVERRIDES() \
    void* operator new(std::size_t size) { return Lumina::Memory::Malloc(size); } \
    void operator delete(void* ptr) noexcept { Lumina::Memory::Free(ptr); } \
    void* operator new[](std::size_t size) { return Lumina::Memory::Malloc(size); } \
    void operator delete[](void* ptr) noexcept { Lumina::Memory::Free(ptr); } \
    void* operator new(std::size_t size, std::align_val_t align) { return Lumina::Memory::Malloc(size, static_cast<size_t>(align)); } \
    void* operator new[](std::size_t size, std::align_val_t align) { return Lumina::Memory::Malloc(size, static_cast<size_t>(align)); } \
    void operator delete(void* ptr, std::align_val_t) noexcept { Lumina::Memory::Free(ptr); } \
    void operator delete[](void* ptr, std::align_val_t) noexcept { Lumina::Memory::Free(ptr); } \
    void* operator new(std::size_t size, const std::nothrow_t&) noexcept { return Lumina::Memory::Malloc(size); } \
    void* operator new[](std::size_t size, const std::nothrow_t&) noexcept { return Lumina::Memory::Malloc(size); } \
    void operator delete(void* ptr, const std::nothrow_t&) noexcept { Lumina::Memory::Free(ptr); } \
    void operator delete[](void* ptr, const std::nothrow_t&) noexcept { Lumina::Memory::Free(ptr); } \
    void operator delete(void* ptr, std::size_t) noexcept { Lumina::Memory::Free(ptr); } \
    void operator delete[](void* ptr, std::size_t) noexcept { Lumina::Memory::Free(ptr); } \
    void operator delete(void* ptr, std::size_t, std::align_val_t) noexcept { Lumina::Memory::Free(ptr); } \
    void operator delete[](void* ptr, std::size_t, std::align_val_t) noexcept { Lumina::Memory::Free(ptr); } \




