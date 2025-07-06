#pragma once
#include "Component.h"
#include "glm/vec4.hpp"

namespace Lumina
{
    struct FPointLightComponent : FEntityComponent
    {
        glm::vec4 LightColor = glm::vec4(1.0f);
    };
}