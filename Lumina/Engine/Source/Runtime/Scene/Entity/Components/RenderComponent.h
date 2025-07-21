#pragma once
#include "Component.h"
#include "Platform/GenericPlatform.h"


namespace Lumina
{
    struct FRenderComponent : FEntityComponent
    {
        TVector<int64> ProxyIDs;
    };

    struct FNeedsRenderProxyUpdate
    {
    };
}
