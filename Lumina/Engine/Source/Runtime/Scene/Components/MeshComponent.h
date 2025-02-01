#pragma once

#include "Assets/AssetHandle.h"
#include "Assets/AssetTypes/MaterialInstance/MaterialInstance.h"
#include "Assets/AssetTypes/StaticMesh/StaticMesh.h"

namespace Lumina
{
    struct FMeshComponent
    {
        TAssetHandle<AStaticMesh> StaticMesh;
        TAssetHandle<AMaterialInstance> Material;
    };
}
