#pragma once
#include "Component.h"
#include <glm/glm.hpp>

#include "EntityComponentRegistry.h"
#include "VelocityComponent.generated.h"

namespace Lumina
{
    LUM_STRUCT()
    struct LUMINA_API SVelocityComponent : SEntityComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT(SVelocityComponent)

        LUM_PROPERTY(ReadOnly)
        glm::vec3 Velocity;

        LUM_PROPERTY(Editable)
        float Speed;
    };

    REGISTER_ENTITY_COMPONENT(SVelocityComponent);

}
