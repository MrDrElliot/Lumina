#pragma once

#include "EntityComponentRegistry.h"
#include "MeshComponent.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "StaticMeshComponent.generated.h"

namespace Lumina
{
    struct FStaticMeshRenderProxy;

    
    LUM_STRUCT()
    struct LUMINA_API SStaticMeshComponent : SMeshComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT()

        
        void SetStaticMesh(const TObjectHandle<CStaticMesh>& InMesh) { StaticMesh = InMesh; }
        TObjectHandle<CStaticMesh> GetStaticMesh() const { return StaticMesh; }

        CMaterialInterface* GetMaterialForSlot(SIZE_T Slot);
        
        LUM_PROPERTY(Editable, Getter, Setter, Category = "Mesh")
        TObjectHandle<CStaticMesh> StaticMesh;

        /** Proxy for this mesh component */
        FStaticMeshRenderProxy* Proxy = nullptr;
    };

    REGISTER_ENTITY_COMPONENT(SStaticMeshComponent);
    
}
