#pragma once
#include "Source/Runtime/Assets/Asset.h"
#include "Source/Runtime/Renderer/Vulkan/VulkanTypes.h"

namespace Lumina
{


    class FStaticMesh : FAsset
    {
    public:

        FStaticMesh(const std::filesystem::path& InPath);
        ~FStaticMesh();


        void SetName(const std::string& NewName);

        std::vector<FGeometrySurface>& GetSurfaces() { return Surfaces; }
        FGPUMeshBuffers& GetMeshBuffers() { return MeshBuffers; }

    private:

        std::string Name;
        std::vector<FGeometrySurface> Surfaces;
        FGPUMeshBuffers MeshBuffers;
    };
}
