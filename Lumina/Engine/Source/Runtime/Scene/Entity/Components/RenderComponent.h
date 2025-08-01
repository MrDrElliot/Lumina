#pragma once

#include "Component.h"
#include "RenderComponent.generated.h"

namespace Lumina
{
    LUM_STRUCT()
    struct LUMINA_API SRenderComponent : SEntityComponent
    {
        GENERATED_BODY()
        
        
    };


    struct FDirtyRenderStateComponent
    {
        uint8 bNeedsTransformUpdate:1 = 0;
        uint8 bNeedsRenderProxyUpdate:1 = 0;
    };
}
