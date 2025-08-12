#pragma once
#include "Component.h"
#include "EntityComponentRegistry.h"
#include "Core/Object/ObjectMacros.h"
#include "EnviormentComponent.generated.h"

namespace Lumina
{
    LUM_STRUCT()
    struct LUMINA_API SSSAOInfo
    {
        GENERATED_BODY()
        
        LUM_PROPERTY(Editable, Category = "SSAO")
        float Radius = 1.0f;

        LUM_PROPERTY(Editable, Category = "SSAO")
        float Intensity = 2.0f;

        LUM_PROPERTY(Editable, Category = "SSAO")
        float Power = 1.5f;
    };

    LUM_STRUCT()
    struct LUMINA_API SAmbientLight
    {
        GENERATED_BODY()
        
        LUM_PROPERTY(Editable, Color, Category = "Ambient Light")
        glm::vec4 Color = glm::vec4(1.0f);
        
    };
    
    LUM_STRUCT()
    struct LUMINA_API SEnviormentComponent : SEntityComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT()
        
        LUM_PROPERTY(Editable, Category = "Lighting")
        SAmbientLight AmbientLight;
        
        LUM_PROPERTY(Editable, Category = "SSAO")
        bool bSSAOEnabled = false;

        LUM_PROPERTY(Editable, Category = "SSAO")
        SSSAOInfo SSAOInfo;
        
    };

    REGISTER_ENTITY_COMPONENT(SEnviormentComponent);
}
