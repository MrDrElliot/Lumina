#pragma once

#include "Assets/Asset.h"
#include "Assets/AssetHandle.h"
#include "Renderer/Buffer.h"
#include "Renderer/MeshData.h"


namespace Lumina
{
    class AMaterialInstance;
}

namespace Lumina
{
    class FMesh : public IAsset
    {
        
    public:
        FMesh(const FAssetPath& InPath)
            : IAsset(InPath)
        {}

        FORCEINLINE const FMeshAsset& GetMeshData() { return MeshData; }
        
        FORCEINLINE const TRefPtr<FBuffer>& GetVertexBuffer()   { return VBO; }
        FORCEINLINE const TRefPtr<FBuffer>& GetIndexBuffer()    { return IBO; }

        FORCEINLINE const uint32 GetNumIndicies() const { return (uint32)MeshData.Indices.size(); }
        FORCEINLINE const uint32 GetNumVertices() const { return (uint32)MeshData.Indices.size(); }
        
        void PostLoad() override;
        
        void Serialize(FArchive& Ar) override
        {
            Ar << MeshData;
        }
        
    private:

        
        FMeshAsset                                  MeshData = {};
        TVector<TAssetHandle<AMaterialInstance>>    Materials;
        TRefPtr<FBuffer>                            VBO;
        TRefPtr<FBuffer>                            IBO;
    };
    
}
