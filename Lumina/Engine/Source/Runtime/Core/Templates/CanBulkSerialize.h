#pragma once
#include <type_traits>

template <typename T>
struct TCanBulkSerialize
{
	enum { Value = std::is_arithmetic_v<T> };
};
