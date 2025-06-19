#pragma once
#include "Memory/RefCounted.h"
#include "Platform/GenericPlatform.h"
#include "Platform/WindowsPlatform.h"


namespace Lumina
{
    template <typename T>
    FORCEINLINE constexpr T Align(T Val, uint64 Alignment)
    {
        static_assert(eastl::is_integral_v<T> || eastl::is_pointer_v<T>, "Align expects an integer or pointer type");

        return (T)(((uint64)Val + Alignment - 1) & ~(Alignment - 1));
    }
}
