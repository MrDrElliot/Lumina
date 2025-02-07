#pragma once

#include "Assets/AssetHandle.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Assets/AssetTypes/MaterialInstance/MaterialInstance.h"

namespace Lumina
{
    struct FMeshComponent
    {
        TAssetHandle<AStaticMesh> StaticMesh;
        TAssetHandle<AMaterialInstance> Material;
    };
}
