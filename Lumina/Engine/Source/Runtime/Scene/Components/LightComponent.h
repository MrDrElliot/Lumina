#pragma once
#include "glm/vec4.hpp"


struct FLightComponent
{
    
    struct FLightData
    {
        glm::vec4 LightPosition =   {0.0f, 0.0f, 0.0f, 0.0f};
        glm::vec4 CameraPosition =  {0.0f, 0.0f, 0.0f, 0.0f};
        glm::vec4 LightColor =      {1.0f, 1.0f, 1.0f, 1.0f};
    } FLightData;
};
