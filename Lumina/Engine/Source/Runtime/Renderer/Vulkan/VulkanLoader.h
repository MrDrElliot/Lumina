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

    struct FGeoSurface
    {
        uint32_t StartIndex;
        uint32_t Count;
    };

    struct FMeshAsset
    {
        std::string Name;

        std::vector<FGeoSurface> Surfaces;
        FGPUMeshBuffers MeshBuffers;
    };

    
    std::optional<std::vector<std::shared_ptr<FMeshAsset>>> LoadGltfMeshes(std::filesystem::path InPath);
}
