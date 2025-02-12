#pragma once

#include "Component.h"
#include "Assets/AssetHandle.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Assets/AssetTypes/MaterialInstance/MaterialInstance.h"

namespace Lumina
{
    struct FMeshComponent : public FEntityComponent
    {
        TAssetHandle<AStaticMesh> StaticMesh;
        TAssetHandle<AMaterialInstance> Material;
    };
}
