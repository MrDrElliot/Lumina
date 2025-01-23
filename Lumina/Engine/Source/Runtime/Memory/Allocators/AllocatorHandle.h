#pragma once
#include "Allocator.h"
#include "Platform/GenericPlatform.h"


class FAllocator;

template<typename T>
class TAllocatorHandle
{
public:
    
    TAllocatorHandle(uint32 InHandle): InternalHandle(InHandle) {}
    
private:

    uint32 InternalHandle;
};
