#pragma once
#include "Component.h"
#include "RenderComponent.h"
#include "glm/vec4.hpp"

namespace Lumina
{
    struct FPointLightComponent : FRenderComponent
    {
        glm::vec4 LightColor = glm::vec4(1.0f);
    };

    struct FDirectionalLightComponent : FRenderComponent
    {
        glm::vec4 Direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
        glm::vec4 Color = glm::vec4(1.0f);
        bool CastShadows = true;
    };
}