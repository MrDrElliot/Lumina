#pragma once

#include "RenderComponent.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "MeshComponent.generated.h"

namespace Lumina
{
    class CMaterialInterface;
    
    LUM_STRUCT()
    struct SMeshComponent : SRenderComponent
    {
        GENERATED_BODY()

        void SetMaterialOverrides(const TVector<TObjectHandle<CMaterialInterface>>& Overrides)
        {
            MaterialOverrides = Overrides;
        }
        
        LUM_PROPERTY(Editable, Setter, Category = "Materials")
        TVector<TObjectHandle<CMaterialInterface>> MaterialOverrides;
    };
    
}
