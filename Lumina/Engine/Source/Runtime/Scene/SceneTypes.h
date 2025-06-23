#pragma once

#include "Containers/Array.h"
#include "Platform/GenericPlatform.h"

namespace Lumina
{
    class FScene;

    enum class ESceneType : uint8
    {
        Game,
        Tool,
    };

    struct FSceneRenderData final
    {
        FSceneRenderData() = default;
        FSceneRenderData(const FSceneRenderData&) = delete;
        const FSceneRenderData& operator = (const FSceneRenderData&) = delete;
        
    };
    
}
