#pragma once
#include "Platform/GenericPlatform.h"
#include "Platform/WindowsPlatform.h"


namespace Lumina
{
    template <typename T>
    requires (eastl::is_integral_v<T> || eastl::is_pointer_v<T>)
    constexpr T Align(T Val, uint64 Alignment)
    {
        return (T)(((uint64)Val + Alignment - 1) & ~(Alignment - 1));
    }
}
