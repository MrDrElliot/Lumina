#pragma once

#include <cstdint>

namespace Lumina
{
    enum class EAssetType : uint8_t
    {
        StaticMesh,
        Scene,
    };

    struct FGeometrySurface
    {
        uint32_t StartIndex;
        uint32_t Count;
    };
    
}
