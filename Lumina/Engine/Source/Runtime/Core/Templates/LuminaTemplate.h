#pragma once
#include <type_traits>


template <typename T>
T ImplicitConv(std::type_identity_t<T> Obj)
{
    return Obj;
}
