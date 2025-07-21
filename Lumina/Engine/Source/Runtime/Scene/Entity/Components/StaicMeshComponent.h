#pragma once

#include "MeshComponent.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"

namespace Lumina
{
    struct FStaticMeshComponent : public FMeshComponent
    {
        CStaticMesh* StaticMesh = nullptr;
    };
}
