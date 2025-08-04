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
        float Intensity = 10.0f;

        LUM_PROPERTY(Editable, Category = "Light")
        float Attenuation = 100.0f;
        
    };

    LUM_STRUCT()
    struct LUMINA_API SSpotLightComponent : SRenderComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT()

        LUM_PROPERTY(Editable, Color, Category = "Light")
        glm::vec3 LightColor = glm::vec3(1.0f);

        LUM_PROPERTY(Editable, Category = "Light")
        float Intensity = 10.0f;

        LUM_PROPERTY(Editable, Category = "Light")
        float InnerConeAngle = glm::radians(20.0f);

        LUM_PROPERTY(Editable, Category = "Light")
        float OuterConeAngle = glm::radians(30.0f);

        LUM_PROPERTY(Editable, Category = "Light")
        float Attenuation = 1.0f;

        LUM_PROPERTY(Editable, Category = "Shadows")
        bool bCastShadows = true;

        LUM_PROPERTY(Editable, Category = "Shadows")
        float ShadowBias = 0.005f;

        LUM_PROPERTY(Editable, Category = "Shadows")
        float ShadowRadius = 1.0f;

        LUM_PROPERTY(Editable, Category = "Advanced")
        bool bVolumetric = false;

        LUM_PROPERTY(Editable, Category = "Advanced")
        float VolumetricIntensity = 0.5f;

        LUM_PROPERTY(Transient)
        uint32 ShadowMapIndex = ~0u;
    };


    LUM_STRUCT()
    struct LUMINA_API SDirectionalLightComponent : SRenderComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT()

        LUM_PROPERTY(Editable, Color, Category = "Light")
        glm::vec3 Color = glm::vec4(1.0f);

        LUM_PROPERTY(Editable, Category = "Light")
        float Intensity = 10.0f;

        LUM_PROPERTY(Editable)
        bool bCastShadows;
    };

    DECLARE_ENTITY_COMPONENT(SPointLightComponent);
    DECLARE_ENTITY_COMPONENT(SDirectionalLightComponent);
    DECLARE_ENTITY_COMPONENT(SSpotLightComponent);
}