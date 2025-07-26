#pragma once

#include "MeshComponent.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "StaticMeshComponent.generated.h"

namespace Lumina
{
    LUM_STRUCT()
    struct LUMINA_API SStaticMeshComponent : SMeshComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT()

        
        LUM_PROPERTY(Editable, Category = "Mesh")
        CStaticMesh* StaticMesh = nullptr;
    };

    DECLARE_ENTITY_COMPONENT(SStaticMeshComponent);
}
