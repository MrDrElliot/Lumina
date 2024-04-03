#pragma once

#include "Source/Runtime/CoreObject/Object.h"
#include <stdint.h>

namespace Lumina
{
	template<typename T>
	class TSharedPtr
	{
	public:
		TSharedPtr()
		{
			Instance = nullptr;
		}

		TSharedPtr(std::nullptr_t n)
		{
			Instance = nullptr;
		}

		TSharedPtr(T* InInstance)
		{
			static_assert(std::is_base_of<UObject, T>::value, "Class is not a UObject!");
			Instance = InInstance;
		}

	private:

		template<class T2>
		friend class TSharedPtr;
		mutable T* Instance;
	};
}