#pragma once

#include "Renderer/Material.h"
#include "Assets/AssetHandle.h"
#include "Assets/AssetTypes/StaticMesh/StaticMesh.h"

namespace Lumina
{

    class FMeshComponent
    {
    public:

        FMeshComponent() = default;
        FMeshComponent(TAssetHandle<LStaticMesh> MeshAsset, TAssetHandle<LMaterial> InMaterial)
            :StaticMesh(MeshAsset), Material(InMaterial) 
        {}

        void SetMeshAsset(const TAssetHandle<LStaticMesh>& Mesh)        { StaticMesh = Mesh; Material = TAssetHandle<LMaterial>(); }
        void SetMaterialAsset(const TAssetHandle<LMaterial>& InMaterial) { Material = InMaterial; }

        TAssetHandle<LStaticMesh> StaticMesh;
        TAssetHandle<LMaterial> Material;
    };
}
