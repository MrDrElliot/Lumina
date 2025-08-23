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
        ENTITY_COMPONENT()
        
        void SetStaticMesh(const TObjectHandle<CStaticMesh>& InMesh)
        {
            if (InMesh == StaticMesh)
            {
                return;
            }
            
            StaticMesh = InMesh;
        }
        
        
        TObjectHandle<CStaticMesh> GetStaticMesh() const { return StaticMesh; }

        CMaterialInterface* GetMaterialForSlot(SIZE_T Slot);
        
        LUM_PROPERTY(Editable, Getter, Setter, Category = "Mesh")
        TObjectHandle<CStaticMesh> StaticMesh;
        
    };

    REGISTER_ENTITY_COMPONENT(SStaticMeshComponent);

}
