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

        LUM_PROPERTY(Editable, Category = "Light", ClampMin = 0.0f)
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

        LUM_PROPERTY(Editable, Category = "Light", ClampMin = 0.0f, ClampMax = 1000.0f)
        float Intensity = 10.0f;

        LUM_PROPERTY(Editable, Category = "Light", ClampMin = 0.0f)
        float InnerConeAngle = 20.0f;

        LUM_PROPERTY(Editable, Category = "Light", ClampMin = 0.0f)
        float OuterConeAngle = 30.0f;

        LUM_PROPERTY(Editable, Category = "Light", ClampMin = 0.0f)
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
        glm::vec3 Direction = glm::vec3(0.0f, 0.3f, 0.8f);

        LUM_PROPERTY(Editable, Category = "Light", ClampMin = 0.0f)
        float Intensity = 10.0f;

        LUM_PROPERTY(Editable)
        bool bCastShadows;
    };

    REGISTER_ENTITY_COMPONENT(SPointLightComponent);
    REGISTER_ENTITY_COMPONENT(SSpotLightComponent);
    REGISTER_ENTITY_COMPONENT(SDirectionalLightComponent);

}