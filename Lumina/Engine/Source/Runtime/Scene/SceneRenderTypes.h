#pragma once

#include <glm/glm.hpp>

#include "Core/Math/Transform.h"
#include "Platform/GenericPlatform.h"
#include "Renderer/RenderResource.h"
#include "Renderer/RHIFwd.h"

#define MAX_LIGHTS 10000
#define MAX_MODELS 10000

namespace Lumina
{
    struct FVertex;
    class CMaterial;
    class CStaticMesh;
}

namespace Lumina
{
}

namespace Lumina
{
    enum class ESceneRenderGBuffer : uint8
    {
        RenderTarget,
        Position,
        Normals,
        Material,
    };

    struct FRenderProxy
    {
        CStaticMesh* Mesh;
        CMaterial* Material;
        glm::mat4 Matrix;
        SIZE_T ProxyID;
        SIZE_T SortKey;
        SIZE_T ModelMatrixIndex;

        bool operator < (const FRenderProxy& Other) const
        {
            return SortKey < Other.SortKey;
        }
    };
    
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
        SIZE_T Key;
        FDrawIndexedIndirectArguments Args;
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
        FSceneRenderData& operator=(const FSceneRenderData&) = delete;

        FSceneRenderData(FSceneRenderData&&) noexcept = default;
        FSceneRenderData& operator=(FSceneRenderData&&) noexcept = default;

        TVector<FDrawIndexedIndirectArguments>  DrawIndexedArguments;
        TVector<FVertex>                        Vertices;
        FSceneLightData                         LightData;
    };


    struct FSceneGlobalData
    {
        FCameraData    CameraData;
        float          Time;
        float          DeltaTime;
        
    };


}
