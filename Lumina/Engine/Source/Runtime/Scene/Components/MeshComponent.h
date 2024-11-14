#pragma once
#include "Memory/RefCounted.h"
#include "Renderer/Material.h"
#include "Assets/AssetHandle.h"

namespace Lumina
{
    class FBuffer;
    class LStaticMesh;
    class Material;

    struct MeshComponent
    {

        MeshComponent() = default;
        MeshComponent(const TAssetHandle<LStaticMesh>& MeshAsset, const TAssetHandle<Material>& InMaterial)
            :StaticMesh(MeshAsset), Material(InMaterial) 
        {}

        TAssetHandle<LStaticMesh> StaticMesh;
        TAssetHandle<Material> Material;
    };
}
