#pragma once

#include "Component.h"
#include "ComponentRegistry.h"
#include "Containers/Name.h"
#include "NameComponent.generated.h"

namespace Lumina
{
    LUM_STRUCT()
    struct LUMINA_API SNameComponent : SEntityComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT()        

        LUM_PROPERTY(ReadOnly)
        FName Name;
    };
    
}
