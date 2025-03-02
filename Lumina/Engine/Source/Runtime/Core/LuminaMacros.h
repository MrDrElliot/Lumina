#pragma once

#include "Containers/Name.h"

#define NAME_None None

#define ENUM_BITSET(Type) \
inline Type operator|(Type lhs, Type rhs) { return static_cast<Type>(static_cast<std::underlying_type<Type>::type>(lhs) | static_cast<std::underlying_type<Type>::type>(rhs)); } \
inline Type operator&(Type lhs, Type rhs) { return static_cast<Type>(static_cast<std::underlying_type<Type>::type>(lhs) & static_cast<std::underlying_type<Type>::type>(rhs)); } \
inline Type operator^(Type lhs, Type rhs) { return static_cast<Type>(static_cast<std::underlying_type<Type>::type>(lhs) ^ static_cast<std::underlying_type<Type>::type>(rhs)); } \
inline Type operator~(Type lhs) { return static_cast<Type>(~static_cast<std::underlying_type<Type>::type>(lhs)); } \
inline Type& operator|=(Type& lhs, Type rhs) { lhs = lhs | rhs; return lhs; } \
inline Type& operator&=(Type& lhs, Type rhs) { lhs = lhs & rhs; return lhs; } \
inline Type& operator^=(Type& lhs, Type rhs) { lhs = lhs ^ rhs; return lhs; }

