#pragma once

template <typename T>
struct TCanBulkSerialize
{
	enum { Value = eastl::is_arithmetic_v<T> };
};
