#pragma once

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
        
        
        FString Name;
        TVector<FVertex> Vertices;
        TVector<uint32> Indices;
        
    };
    

    class AStaticMesh : public IAsset
    {
    public:

        AStaticMesh() = default;
        AStaticMesh(FMeshAsset InInfo);
        virtual ~AStaticMesh() override;

        DECLARE_ASSET("Static Mesh", StaticMesh, 1);
        
        static TSharedPtr<AStaticMesh> CreateMesh(FMeshAsset InInfo);

        void CreateNew();
        
        FMeshAsset& GetMeshData() { return MeshData; }
        TRefPtr<FBuffer>& GetVertexBuffer() { return VBO; }
        TRefPtr<FBuffer>& GetIndexBuffer() { return IBO; }

        void Serialize(FArchive& Ar) override;

    private:
        
        FMeshAsset          MeshData = {};
        TRefPtr<FBuffer>    VBO;
        TRefPtr<FBuffer>    IBO;
    };
}
