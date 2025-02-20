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
    class AMesh : public IAsset
    {
        
    public:
        
        AMesh(const FAssetPath& InPath)
            : IAsset(InPath)
        {}

        void PostLoad() override;
        
        void Serialize(FArchive& Ar) override
        {
            Ar << MeshResource;
        }

        
        FORCEINLINE const FMeshResource& GetMeshResource() const { return MeshResource; }
        FORCEINLINE const uint32 GetNumIndicies() const { return (uint32)MeshResource.Indices.size(); }
        FORCEINLINE const uint32 GetNumVertices() const { return (uint32)MeshResource.Indices.size(); }
        FORCEINLINE const TVector<TAssetHandle<AMaterialInstance>>& GetMaterials() const { return Materials; }

        void Bind();

        
    private:
        
        FMeshResource                               MeshResource = {};
        TVector<TAssetHandle<AMaterialInstance>>    Materials;
    };
    
}
