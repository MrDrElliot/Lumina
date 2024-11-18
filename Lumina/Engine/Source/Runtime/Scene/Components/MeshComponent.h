#pragma once

#include "Renderer/Material.h"
#include "Assets/AssetHandle.h"

namespace Lumina
{
    class FBuffer;
    class LStaticMesh;
    class Material;

    class FMeshComponent
    {
    public:

        FMeshComponent() = default;
        FMeshComponent(const TAssetHandle<LStaticMesh>& MeshAsset, const TAssetHandle<Material>& InMaterial)
            :StaticMesh(MeshAsset), Material(InMaterial) 
        {}

        TAssetHandle<LStaticMesh> StaticMesh;
        TAssetHandle<Material> Material;
    };
}
