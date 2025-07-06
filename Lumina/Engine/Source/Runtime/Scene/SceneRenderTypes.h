#pragma once

#include <glm/glm.hpp>

#include "Platform/GenericPlatform.h"
#include "Renderer/RHIFwd.h"

#define MAX_LIGHTS 200

namespace Lumina
{
    class CStaticMesh;
    
    struct FCameraData
    {
        glm::vec4 Location =    {};
        glm::mat4 View =        {};
        glm::mat4 Projection =  {};
    };

    struct FPointLight
    {
        glm::vec4 Position =   glm::vec4(0.0f);
        glm::vec4 Color =      glm::vec4(0.0f);
    };

    struct FSceneLightData
    {
        uint32 NumLights =  0;
        uint32 padding[3] = {0,0,0};
        FPointLight Lights[MAX_LIGHTS];
    };

    struct FModelData
    {
        glm::mat4 ModelMatrix;
    };

    struct FGBuffer
    {
        FRHIImageRef Position;
        FRHIImageRef Normals;
        FRHIImageRef Material;
        FRHIImageRef AlbedoSpec;
    };

    struct FSceneRenderData final
    {
        FSceneRenderData() = default;
        FSceneRenderData(const FSceneRenderData&) = delete;
        const FSceneRenderData& operator = (const FSceneRenderData&) = delete;

        TVector<CStaticMesh*>   Meshes;
        TVector<FModelData>     ModelData;
        FSceneLightData         LightData;
        
    };

    struct FSceneGlobalData
    {
        FCameraData    CameraData;
        float          Time;
        float          DeltaTime;
    };


}
