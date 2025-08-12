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

        LUM_PROPERTY(Editable, Category = "Materials")
        TVector<TObjectHandle<CMaterialInterface>> MaterialOverrides;
    };
    
}
