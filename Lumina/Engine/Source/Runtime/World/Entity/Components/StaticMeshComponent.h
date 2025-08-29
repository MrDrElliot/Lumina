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
        
        void SetStaticMesh(const TObjectHandle<CStaticMesh>& InMesh)
        {
            StaticMesh = InMesh;
        }
        
        
        TObjectHandle<CStaticMesh> GetStaticMesh() const { return StaticMesh; }

        CMaterialInterface* GetMaterialForSlot(SIZE_T Slot) const;
        
        LUM_PROPERTY(Editable, Getter, Setter, Category = "Mesh")
        TObjectHandle<CStaticMesh> StaticMesh;
        
    };
}
