#pragma once
#include "Component.h"
#include <glm/glm.hpp>

namespace Lumina
{
    struct FVelocityComponent : FEntityComponent
    {
        glm::vec3 Velocity;
        float Speed;
    };
}
