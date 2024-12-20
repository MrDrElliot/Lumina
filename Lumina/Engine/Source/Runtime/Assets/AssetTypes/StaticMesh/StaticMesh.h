#pragma once
#include <filesystem>

#include "Renderer/Buffer.h"
#include "Memory/RefCounted.h"
#include "Assets/Asset.h"
#include "Containers/Array.h"
#include "Renderer/RenderTypes.h"


namespace Lumina
{
    class FBuffer;

    struct FMeshAsset
    {

        FMeshAsset() = default;
        
        FMeshAsset(FMeshAsset&& Other) noexcept
        : Name(std::move(Other.Name))
        , Vertices(std::move(Other.Vertices))
        , Indices(std::move(Other.Indices))
        {
            Other.Name.clear();
            Other.Vertices.clear();
            Other.Indices.clear();
        }

        FMeshAsset(const FMeshAsset&) = delete;

        
        friend FArchive& operator << (FArchive& Ar, FMeshAsset& data)
        {
            Ar << data.Name;
            Ar << data.Vertices;
            Ar << data.Indices;

            return Ar;
        }
        
        
        std::string Name;
        TFastVector<FVertex> Vertices;
        TFastVector<uint32> Indices;
        
    };
    

    class LStaticMesh : public LAsset
    {
    public:

        LStaticMesh() = default;
        LStaticMesh(const FAssetMetadata& Metadata, FMeshAsset InInfo);
        static std::shared_ptr<LStaticMesh> CreateMesh(const FAssetMetadata& Metadata, FMeshAsset InInfo);

        void CreateNew();
        
        void Destroy();

        FMeshAsset& GetMeshData() { return MeshData; }
        TRefPtr<FBuffer>& GetVertexBuffer() { return VBO; }
        TRefPtr<FBuffer>& GetIndexBuffer() { return IBO; }

        void Serialize(FArchive& Ar) override;

    private:
        
        FMeshAsset MeshData;
        TRefPtr<FBuffer> VBO;
        TRefPtr<FBuffer> IBO;
    };
}
