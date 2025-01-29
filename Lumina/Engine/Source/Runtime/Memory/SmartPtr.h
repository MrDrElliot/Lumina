#pragma once

#include "EASTL/shared_ptr.h"
#include "EASTL/unique_ptr.h"

namespace Lumina
{
    template<typename S> using TSharedPtr = eastl::shared_ptr<S>;
    template<typename S> using TUniquePtr = eastl::unique_ptr<S>;

    template<typename T, typename... Args>
    TSharedPtr<T> MakeSharedPtr(Args&&... args)
    {
        static_assert(std::is_constructible_v<T, Args&&...>, "Cannot instantiate T with Args");

        return eastl::make_shared<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    TUniquePtr<T> MakeUniquePtr(Args&&... args)
    {
        static_assert(std::is_constructible_v<T, Args&&...>, "Cannot instantiate T with Args");

        return eastl::make_unique<T>(std::forward<Args>(args)...);
    }
}
