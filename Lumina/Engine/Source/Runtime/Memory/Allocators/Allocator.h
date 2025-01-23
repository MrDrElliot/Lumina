#pragma once

#include <memory>

#include "Core/Singleton/Singleton.h"
#include "Platform/GenericPlatform.h"
#include "AllocatorHandle.h"

class FAllocator : public TSingleton<FAllocator>
{

public:

    void Initialize(size_t InSize): mSize(InSize), mMemoryBlock(new char[mSize]) {}


    template<typename T>
    TAllocatorHandle<T> Allocate(size_t Size = sizeof(T), size_t Alignment = alignof(T))
    {
        void* CurrentAddress = mMemoryBlock + mOffset;
        size_t Space = mSize - mOffset;
        void* AlignedAddress = std::align(Alignment, Size, CurrentAddress, Space);

        mOffset = static_cast<char*>(AlignedAddress) - mMemoryBlock + Size;

        return TAllocatorHandle<T>(mOffset);
    }

    void Deallocate(uint32 Handle)
    {
        
    }

    void Reset()
    {
        
    }

private:

    char* mMemoryBlock =        nullptr;
    size_t mSize =              0;
    size_t mOffset =            0;
    uint32 mCurrentVersion =    0;
    
};
