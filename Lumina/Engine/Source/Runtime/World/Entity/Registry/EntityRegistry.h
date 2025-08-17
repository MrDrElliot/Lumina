#pragma once
#include <entt/entt.hpp>
#include "Memory/Memory.h"

namespace Lumina
{

    template<typename T>
    struct TEnttAllocator
    {
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        TEnttAllocator() = default;

        template<typename U>
        TEnttAllocator(const TEnttAllocator<U>&) {}

        T* allocate(std::size_t n)
        {
            void* ptr = Memory::Malloc(n * sizeof(T), alignof(T));
            if (!ptr) throw std::bad_alloc();
            return static_cast<T*>(ptr);
        }

        void deallocate(T* p, std::size_t)
        {
            Memory::Free(p);
        }
    };

    // required by allocator model
    template<typename T, typename U>
    bool operator==(const TEnttAllocator<T>&, const TEnttAllocator<U>&) { return true; }

    template<typename T, typename U>
    bool operator!=(const TEnttAllocator<T>&, const TEnttAllocator<U>&) { return false; }

    
    class FEntityRegistry : public entt::basic_registry<entt::entity, TEnttAllocator<entt::entity>>
    {
    public:

        FEntityRegistry() = default;
        ~FEntityRegistry() = default;
    
    private:
        
    };
}
