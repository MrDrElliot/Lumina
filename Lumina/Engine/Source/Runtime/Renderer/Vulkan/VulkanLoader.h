#pragma once
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "VulkanTypes.h"
#include <iostream>
#include <optional>


namespace Lumina
{
    class FVulkanRenderContext;

    struct FGGeoSurface
    {
        uint32_t StartIndex;
    };

    struct FMeshAsset
    {
        std::string Name;

        std::vector<FGGeoSurface> Surfaces;
        FGPUMeshBuffers MeshBuffers;
    };

    
    std::optional<std::vector<std::shared_ptr<FMeshAsset>>> LoadGltfMeshes(std::filesystem::path InPath);
}
