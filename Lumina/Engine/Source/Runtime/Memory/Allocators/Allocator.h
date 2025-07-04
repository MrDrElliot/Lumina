#pragma once

#include "Memory/Memory.h"

namespace Lumina
{

    class IAllocator
    {
    public:
        virtual ~IAllocator() = default;

        // Allocates memory of specified size and alignment.
        virtual void* Allocate(SIZE_T Size, SIZE_T Alignment = alignof(std::max_align_t)) = 0;

        virtual void Free(void* Data) = 0;
        
        // Clears or resets the allocator (depending on strategy).
        virtual void Reset() = 0;
    };

    class FDefaultAllocator : public IAllocator
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
    
    class FFrameAllocator : public IAllocator
    {
    public:
        explicit FFrameAllocator(SIZE_T CapacityBytes)
            : Capacity(CapacityBytes)
        {
            Base = (uint8*)Memory::Malloc(Capacity);
            Offset = 0;
        }

        ~FFrameAllocator() override
        {
            Memory::Free(Base);
            Base = nullptr;
        }

        void* Allocate(SIZE_T Size, SIZE_T Alignment = DEFAULT_ALIGNMENT) override
        {
            SIZE_T CurrentPtr = reinterpret_cast<SIZE_T>(Base + Offset);
            SIZE_T AlignedPtr = (CurrentPtr + Alignment - 1) & ~(Alignment - 1);
            SIZE_T NextOffset = AlignedPtr - reinterpret_cast<SIZE_T>(Base) + Size;

            assert(NextOffset <= Capacity && "FFrameAllocator out of memory");

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
        SIZE_T GetCapacity() const { return Capacity; }
        SIZE_T GetRemaining() const { return Capacity - Offset; }

    private:
        
        uint8* Base = nullptr;
        SIZE_T Offset = 0;
        SIZE_T Capacity = 0;
    };

}