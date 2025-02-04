#pragma once

#include "Containers/String.h"
#include "Core/LuminaMacros.h"
#include "Memory/RefCounted.h"

namespace Lumina
{
    class IRenderContext;
}

/**
 * Base class for all render resources. Lifetime is managed internally with intrusive
 * reference counting.
 */
namespace Lumina
{
    class FRenderResource : public FRefCounted
    {
    public:

        virtual void SetFriendlyName(const FString& InName) { FriendlyName = InName; }
        const FString& GetFriendlyName() const { return FriendlyName; }


    private:

        FString FriendlyName = NAME_None;
    };
}