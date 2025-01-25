#pragma once

#include "Containers/String.h"
#include "Core/LuminaMacros.h"
#include "Memory/RefCounted.h"

/**
 * Base class for all render resources. Lifetime is managed internally with intrusive
 * reference counting.
 */
class FRenderResource : public FRefCounted
{
public:

    virtual void SetFriendlyName(const LString& InName) { FriendlyName = InName; }
    const LString& GetFriendlyName() const { return FriendlyName; }


private:

    LString FriendlyName = NAME_None;
};
