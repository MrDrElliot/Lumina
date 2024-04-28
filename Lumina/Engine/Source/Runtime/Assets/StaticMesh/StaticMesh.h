#pragma once
#include <filesystem>

#include "Source/Runtime/Assets/Asset.h"
#include "Source/Runtime/Renderer/RenderTypes.h"


namespace Lumina
{
    class FBuffer;

    struct FMeshAsset
    {
        std::string Name;
        
        std::vector<FVertex> Vertices;
        std::vector<uint32_t> Indices;
        
    };
    

    class LStaticMesh : public LAsset
    {
    public:
        
        LStaticMesh(const FAssetMetadata& Metadata, FMeshAsset InInfo);
        static std::shared_ptr<LStaticMesh> CreateMesh(const FAssetMetadata& Metadata, FMeshAsset InInfo);

        FMeshAsset GetMeshData() { return MeshData; }
        std::pair<std::shared_ptr<FBuffer>, std::shared_ptr<FBuffer>> GetBuffers() { return {VBO, IBO}; }

    private:

        
        FMeshAsset MeshData;
        std::shared_ptr<FBuffer> VBO;
        std::shared_ptr<FBuffer> IBO;
    };
}
