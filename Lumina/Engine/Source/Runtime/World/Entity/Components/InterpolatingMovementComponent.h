#pragma once
#include "Component.h"
#include "Core/Object/ManualReflectTypes.h"
#include "InterpolatingMovementComponent.generated.h"

namespace Lumina
{
    LUM_STRUCT()
    struct LUMINA_API SInterpolatingMovementComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT(SInterpolatingMovementComponent);

        LUM_PROPERTY(Editable, Category = "Interp")
        FTransform Start;
        
        LUM_PROPERTY(Editable, Category = "Interp")
        FTransform End;

        LUM_PROPERTY(Editable, Category = "Interp")
        float Speed = 1.0f;

        float Alpha = 0.0f;
        bool bForward = true;
    };
    
}
