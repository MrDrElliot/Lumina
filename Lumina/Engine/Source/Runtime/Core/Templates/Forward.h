#pragma once
#include "EASTL/type_traits.h"


template <typename T>
constexpr T&& TForward(eastl::remove_reference_t<T>& x) noexcept
{
    return static_cast<T&&>(x);
}


template <typename T>
constexpr T&& TForward(eastl::remove_reference_t<T>&& x) noexcept
{
    static_assert(!eastl::is_lvalue_reference_v<T>, "Forward T isn't lvalue reference");
    return static_cast<T&&>(x);
}