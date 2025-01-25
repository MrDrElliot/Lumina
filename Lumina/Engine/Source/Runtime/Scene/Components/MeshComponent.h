#pragma once

#include "Renderer/Material.h"
#include "Assets/AssetHandle.h"
#include "Assets/AssetTypes/MaterialInstance/MaterialInstance.h"
#include "Assets/AssetTypes/StaticMesh/StaticMesh.h"

namespace Lumina
{
    struct FMeshComponent
    {
        TAssetHandle<LStaticMesh> StaticMesh;
        TAssetHandle<LMaterialInstance> Material;
    };
}
