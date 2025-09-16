#pragma once


#include "MeshComponent.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "StaticMeshComponent.generated.h"

namespace Lumina
{
    LUM_STRUCT()
    struct LUMINA_API SStaticMeshComponent : SMeshComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT(SStaticMeshComponent);
        
        CMaterialInterface* GetMaterialForSlot(SIZE_T Slot) const;
        
        LUM_PROPERTY(Editable, Category = "Mesh")
        TObjectHandle<CStaticMesh> StaticMesh;
        
    };
}
