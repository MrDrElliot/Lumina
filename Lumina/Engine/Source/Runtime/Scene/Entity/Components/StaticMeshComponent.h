#pragma once

#include "MeshComponent.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "StaticMeshComponent.generated.h"
#include "Core/Object/ObjectHandleTyped.h"

namespace Lumina
{
    struct FStaticMeshRenderProxy;

    
    LUM_STRUCT()
    struct LUMINA_API SStaticMeshComponent : SMeshComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT()



        CMaterialInterface* GetMaterialForSlot(SIZE_T Slot);
        
        LUM_PROPERTY(Editable, Category = "Mesh")
        TObjectHandle<CStaticMesh> StaticMesh;

        /** Proxy for this mesh component */
        FStaticMeshRenderProxy* Proxy = nullptr;
    };

    DECLARE_ENTITY_COMPONENT(SStaticMeshComponent);

}
