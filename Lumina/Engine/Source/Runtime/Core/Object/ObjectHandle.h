#pragma once
#include "Module/API.h"
#include "Platform/GenericPlatform.h"
#include "Platform/Platform.h"


namespace Lumina
{
    class CObject;
    class CObjectBase;
}

namespace Lumina
{
    struct FObjectHandle
    {
        uint32 Index = 0;
        uint32 Generation = 0;

        LUMINA_API FObjectHandle() = default;
        LUMINA_API FObjectHandle(uint32 InIndex, uint32 InGeneration);
        LUMINA_API FObjectHandle(const CObject* Object);
        
        LUMINA_API uint64 ToHandle() const
        {
            return (uint64(Generation) >> 32) | Index;
        }

        LUMINA_API static FObjectHandle FromHandle(uint64 Value)
        {
            FObjectHandle Handle;
            Handle.Index = Value & 0xFFFFFFFF;
            Handle.Generation = Value >> 32;
            return Handle;
        }

        NODISCARD bool operator!() const { return Resolve() == nullptr; }
        NODISCARD explicit operator bool() const { return Resolve() != nullptr; }

        
        LUMINA_API CObject* Resolve() const;
    };
    
}
