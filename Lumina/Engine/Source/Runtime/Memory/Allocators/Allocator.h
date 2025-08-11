#pragma once

#include "Memory/Memory.h"

namespace Lumina
{

    class LUMINA_API IAllocator
    {
    public:
        virtual ~IAllocator() = default;

        template<typename T, typename... Args>
        T* TAlloc(Args&&... args)
        {
            void* mem = Allocate(sizeof(T), alignof(T));
            return new (mem) T(std::forward<Args>(args)...);
        } 
        
        // Allocates memory of specified size and alignment.
        virtual void* Allocate(SIZE_T Size, SIZE_T Alignment = alignof(std::max_align_t)) = 0;

        virtual void Free(void* Data) = 0;
        
        // Clears or resets the allocator (depending on strategy).
        virtual void Reset() = 0;
    };

    class LUMINA_API FDefaultAllocator : public IAllocator
    {
    public:
        
        
        void* Allocate(SIZE_T Size, SIZE_T Alignment) override
        {
            return Memory::Malloc(Size, Alignment);
        }
        
        void Free(void* Data) override
        {
            Memory::Free(Data);
        }
        
        void Reset() override { }
    };
    
    class LUMINA_API FLinearAllocator : public IAllocator
    {
    public:
        
        explicit FLinearAllocator(SIZE_T InitialSize)
        {
            Base = (uint8*)Memory::Malloc(InitialSize);
            Offset = 0;
        }

        ~FLinearAllocator() override
        {
            Memory::Free(Base);
            Base = nullptr;
        }
        
        void* Allocate(SIZE_T Size, SIZE_T Alignment = DEFAULT_ALIGNMENT) override
        {
            SIZE_T CurrentPtr = reinterpret_cast<SIZE_T>(Base + Offset);
            SIZE_T AlignedPtr = (CurrentPtr + Alignment - 1) & ~(Alignment - 1);
            SIZE_T NextOffset = AlignedPtr - reinterpret_cast<SIZE_T>(Base) + Size;


            void* Result = Base + (AlignedPtr - reinterpret_cast<SIZE_T>(Base));
            Offset = NextOffset;
            return Result;
        }

        void Free(void* Data) override { }

        void Reset() override
        {
            Offset = 0;
        }

        SIZE_T GetUsed() const { return Offset; }

    private:
        
        uint8* Base = nullptr;
        SIZE_T Offset = 0;
    };

}