#pragma once

#include <glm/glm.hpp>

#include "Platform/GenericPlatform.h"
#include "Renderer/RHIFwd.h"

#define MAX_LIGHTS 10000
#define MAX_MODELS 10000

namespace Lumina
{
    struct FVertex;
}

namespace Lumina
{
    class CMaterial;
}

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

    struct FDirectionalLight
    {
        glm::vec4 Direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
        glm::vec4 Color = glm::vec4(1.0f);
    };

    struct FSceneLightData
    {
        uint32 NumPointLights = 0;
        bool bHasDirectionalLight = false;
        uint32 padding[2];
        FDirectionalLight DirectionalLight;
        FPointLight PointLightsLights[MAX_LIGHTS];
    };

    struct FModelData
    {
        glm::mat4 ModelMatrix[MAX_MODELS];
    };

    struct FGBuffer
    {
        FRHIImageRef Position;
        FRHIImageRef Normals;
        FRHIImageRef Material;
        FRHIImageRef AlbedoSpec;
    };

    struct FIndirectRenderBatch
    {
        CStaticMesh* Mesh;
        CMaterial* Material;
        uint32 VertexOffset;
        uint32 First;
        uint32 Count;
    };

    struct FStaticMeshRender
    {
        CStaticMesh* Mesh;
        CMaterial* Material;
        glm::mat4 Matrix;
    };

    struct FSceneRenderStats
    {
        uint32 NumDrawCalls;
        uint64 NumVertices;
        uint64 NumIndices;
    };

    struct FSceneRenderData final
    {
        FSceneRenderData() = default;
        FSceneRenderData(const FSceneRenderData&) = delete;
        const FSceneRenderData& operator = (const FSceneRenderData&) = delete;

        TVector<FStaticMeshRender>      StaticMeshRenders;
        TVector<FVertex>                Vertices;
        TVector<FIndirectRenderBatch>   RenderBatch;
        FModelData                      ModelData;
        FSceneLightData                 LightData;
        
    };

    struct FSceneGlobalData
    {
        FCameraData    CameraData;
        float          Time;
        float          DeltaTime;
        
    };


}
