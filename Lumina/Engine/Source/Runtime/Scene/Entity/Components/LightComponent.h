#pragma once

#include "RenderComponent.h"
#include "glm/glm.hpp"
#include "LightComponent.generated.h"

namespace Lumina
{
    LUM_STRUCT()
    struct LUMINA_API SPointLightComponent : SRenderComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT()
        
        LUM_PROPERTY(Editable, Color, Category = "Light")
        glm::vec3 LightColor = glm::vec3(1.0f);

        LUM_PROPERTY(Editable, Category = "Light")
        float Intensity = 1.0f;

        LUM_PROPERTY(Editable, Category = "Light")
        float Attenuation = 1.0f;
        
    };

    LUM_STRUCT()
    struct LUMINA_API SDirectionalLightComponent : SRenderComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT()

        LUM_PROPERTY(Editable, Category = "Light")
        glm::vec3 Direction = glm::vec3(1.0f, 0.0f, 0.0f);

        LUM_PROPERTY(Editable, Color, Category = "Light")
        glm::vec3 Color = glm::vec4(1.0f);

        LUM_PROPERTY(Editable, Category = "Light")
        float Intensity = 1.0f;

        LUM_PROPERTY(Editable)
        bool CastShadows;
    };

    DECLARE_ENTITY_COMPONENT(SPointLightComponent);
    DECLARE_ENTITY_COMPONENT(SDirectionalLightComponent);
}