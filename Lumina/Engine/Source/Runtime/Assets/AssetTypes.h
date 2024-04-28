#pragma once

#include <cstdint>

namespace Lumina
{
    enum class EAssetType : uint8_t
    {
        StaticMesh,
        SkeletalMesh,
        Texture,
        Shader,
        Prefab,
        Scene,
    };
    
}
