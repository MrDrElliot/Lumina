#pragma once

#include "MeshComponent.h"
#include "Assets/AssetHandle.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"

namespace Lumina
{
    struct FStaticMeshComponent : public FMeshComponent
    {

        FORCEINLINE bool IsLoaded() const { return StaticMesh.IsLoaded(); }
        
        TAssetHandle<AStaticMesh> StaticMesh;
    };
}
