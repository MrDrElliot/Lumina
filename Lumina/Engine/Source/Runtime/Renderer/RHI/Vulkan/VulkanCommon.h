#pragma once

#include "EASTL/internal/type_compound.h"
#include "Memory/Memory.h"
#include "Platform/GenericPlatform.h"
#include "Platform/WindowsPlatform.h"

namespace Lumina
{
    template<class T>
    static FORCEINLINE void ZeroVkStruct(T& Struct, int32 VkStructureType)
    {
        static_assert(!eastl::is_pointer_v<T>, "Don't use a pointer!");
        static_assert(offsetof(T, sType) == 0, "Assumes sType is the first member in the Vulkan type!");
        static_assert(sizeof(T::sType) == sizeof(int32), "Assumed sType is compatible with int32!");
        
        // Horrible way to coerce the compiler to not have to know what T::sType is so we can have this header not have to include vulkan.h
        (int32&)Struct.sType = VkStructureType;
        FMemory::MemsetZero(((uint8*)&Struct) + sizeof(VkStructureType), sizeof(T) - sizeof(VkStructureType));
    }
}
