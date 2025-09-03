#pragma once

#include <rpmalloc.h>
#include <EASTL/type_traits.h>
#include "Core/Assertions/Assert.h"
#include "Core/Math/Math.h"
#include "Module/API.h"
#include "Platform/Platform.h"
#include "tracy/TracyC.h"

#define LUMINA_RPMALLOC 1

#define LUMINA_PROFILE_ALLOC(p, size) TracyCAllocS(p, size, 12)
#define LUMINA_PROFILE_FREE(p) TracyCFreeS(p, 12)

inline bool GIsMemorySystemInitialized = false;
inline bool GIsMemorySystemShutdown = false;
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
    
    LUMINA_API inline void Memzero(void* ptr, size_t size)
    {
        memset(ptr, 0, size);
    }
    
    template <typename T>
    void Memzero(T* ptr)
    {
        memset(ptr, 0, sizeof(T));
    }

    LUMINA_API inline bool IsAligned(void const* ptr, size_t n)
    {
        return (reinterpret_cast<uintptr_t>(ptr) % n) == 0;
    }

    template <typename T>
    bool IsAligned(T const* p)
    {
        return (reinterpret_cast<uintptr_t>(p) % alignof( T )) == 0;
    }

    LUMINA_API inline void CustomAssert(const char* pMessage)
    {
#if 0
        if (Logging::IsInitialized())
        {
            LOG_CRITICAL("[Memory] - ", pMessage);
        }
        else
        {
            std::cout << "[Memory Error] - " << pMessage << "\n";
        }
#endif
    }

    LUMINA_API void Initialize();

    LUMINA_API void Shutdown();

    LUMINA_API void InitializeThreadHeap();

    LUMINA_API NODISCARD inline bool IsThreadHeapInitialized()
    {
        return rpmalloc_is_thread_initialized();
    }

    LUMINA_API NODISCARD inline void ShutdownThreadHeap()
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

    LUMINA_API NODISCARD SIZE_T GetActualAlignment(size_t size, size_t alignment);

    LUMINA_API void Memcpy(void* Destination, void* Source, uint64 SrcSize);
    
    LUMINA_API void Memcpy(void* Destination, const void* Source, uint64 SrcSize);

    LUMINA_API NODISCARD void* Malloc(size_t size, size_t alignment = DEFAULT_ALIGNMENT);
    
    LUMINA_API NODISCARD void* Realloc(void* Memory, size_t NewSize, size_t OriginalAlignment = DEFAULT_ALIGNMENT);

    LUMINA_API void Free(void*& Memory);

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




