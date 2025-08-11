#pragma once
#include "Containers/Array.h"
#include "Memory/Allocators/Allocator.h"

namespace Lumina
{
    class IAllocator;
}

namespace Lumina
{
    template<typename T>
    class TRGHandleRegistry
    {
    public:

        TRGHandleRegistry() = default;
        TRGHandleRegistry(const TRGHandleRegistry&) = delete;
        TRGHandleRegistry(TRGHandleRegistry&&) = default;
        TRGHandleRegistry& operator=(TRGHandleRegistry&&) = default;
        TRGHandleRegistry& operator=(const TRGHandleRegistry&) = delete;

        ~TRGHandleRegistry() { Clear(); }


        void Insert(T* Object)
        {
            Array.emplace_back(Object);
        }
        
        template<typename DerivedType = T, typename... TArgs>
        requires(std::is_base_of_v<T, DerivedType>)
        DerivedType* Allocate(IAllocator& Allocator, TArgs&&... Args)
        {
            DerivedType* Object = Allocator.TAlloc<DerivedType>(std::forward<TArgs>(Args)...);
            Insert(Object);
            
            return Object;
        }
        
        void Clear()
        {
            for (int32 Index = Array.size() - 1; Index >= 0; --Index)
            {
                Array[Index]->~T();
            }
            
            Array.clear();
        }


        T** begin() { return Array.begin(); }
        T** end() { return Array.end(); }


    private:

        TFixedVector<T*, 12> Array;
        
    };
}
