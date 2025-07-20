#pragma once
#include "Component.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    struct FRenderComponent : public FEntityComponent
    {
        int64 ProxyID = INDEX_NONE;
    };

    struct FNeedsRenderProxyUpdate
    {
    };
}
