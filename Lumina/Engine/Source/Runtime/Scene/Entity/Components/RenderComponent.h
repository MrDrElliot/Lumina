#pragma once
#include "Component.h"
#include "Platform/GenericPlatform.h"
#include "RenderComponent.generated.h"

namespace Lumina
{
    LUM_STRUCT()
    struct SRenderComponent : SEntityComponent
    {
        GENERATED_BODY()
        
        TVector<int64> ProxyIDs;
    };

    struct FNeedsRenderProxyUpdate
    {
    };
}
