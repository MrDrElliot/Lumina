#pragma once

#include "Component.h"
#include "EntityComponentRegistry.h"
#include "Containers/Name.h"
#include "NameComponent.generated.h"

namespace Lumina
{
    LUM_STRUCT()
    struct LUMINA_API SNameComponent : SEntityComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT()        

        LUM_PROPERTY(Editable)
        FName Name;
    };
    
    REGISTER_ENTITY_COMPONENT(SNameComponent);
}
