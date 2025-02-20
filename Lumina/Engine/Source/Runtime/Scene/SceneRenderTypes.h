#pragma once

#include <glm/glm.hpp>

#include "Platform/GenericPlatform.h"
#define MAX_LIGHTS 102


namespace Lumina
{
    struct FCameraData
    {
        glm::vec4 Location =    {};
        glm::mat4 View =        {};
        glm::mat4 Projection =  {};
    };

    struct FLight
    {
        glm::vec4 LightPosition =   glm::vec4(0.0f);
        glm::vec4 LightColor =      glm::vec4(0.0f);
    };

    struct FSceneLightData
    {
        uint32 NumLights =  0;
        uint32 padding[3] = {0,0,0};
        FLight Lights       [MAX_LIGHTS];
    };

    struct FSceneGlobalData
    {
        FCameraData     CameraData;
        double          Time;
        double          DeltaTime;
    };


}
