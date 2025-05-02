#pragma once

#include "Containers/Name.h"

#define NAME_None FName("NAME_None")

#define ENUM_CLASS_FLAGS(Enum) \
inline           Enum& operator|=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
inline           Enum& operator&=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
inline           Enum& operator^=(Enum& Lhs, Enum Rhs) { return Lhs = (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
inline constexpr Enum  operator| (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs | (__underlying_type(Enum))Rhs); } \
inline constexpr Enum  operator& (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs & (__underlying_type(Enum))Rhs); } \
inline constexpr Enum  operator^ (Enum  Lhs, Enum Rhs) { return (Enum)((__underlying_type(Enum))Lhs ^ (__underlying_type(Enum))Rhs); } \
inline constexpr bool  operator! (Enum  E)             { return !(__underlying_type(Enum))E; } \
inline constexpr Enum  operator~ (Enum  E)             { return (Enum)~(__underlying_type(Enum))E; }

template<typename Enum>
constexpr bool EnumHasAllFlags(Enum Flags, Enum Contains)
{
    using UnderlyingType = __underlying_type(Enum);
    return ((UnderlyingType)Flags & (UnderlyingType)Contains) == (UnderlyingType)Contains;
}

template<typename Enum>
constexpr bool EnumHasAnyFlags(Enum Flags, Enum Contains)
{
    using UnderlyingType = __underlying_type(Enum);
    return ((UnderlyingType)Flags & (UnderlyingType)Contains) != 0;
}

template<typename Enum>
void EnumAddFlags(Enum& Flags, Enum FlagsToAdd)
{
    using UnderlyingType = __underlying_type(Enum);
    Flags = (Enum)((UnderlyingType)Flags | (UnderlyingType)FlagsToAdd);
}

template<typename Enum>
void EnumRemoveFlags(Enum& Flags, Enum FlagsToRemove)
{
    using UnderlyingType = __underlying_type(Enum);
    Flags = (Enum)((UnderlyingType)Flags & ~(UnderlyingType)FlagsToRemove);
}