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
        Albedo,
        Position,
        Normals,
        Material,
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
        FPointLight PointLights[MAX_LIGHTS];
    };

    struct FModelData
    {
        TVector<glm::mat4> ModelMatrices;
    };

    struct FGBuffer
    {
        FRHIImageRef Position;
        FRHIImageRef Normals;
        FRHIImageRef Material;
        FRHIImageRef AlbedoSpec;
    };

    struct FRenderProxy
    {
        CStaticMesh*    Mesh = nullptr;
        CMaterial*      Material = nullptr;
        glm::mat4       Matrix;
        SIZE_T          ProxyID = 0;
        SIZE_T          SortKey = 0;
        int64           ModelMatrixIndex = INDEX_NONE;
        
        bool operator < (const FRenderProxy& Other) const
        {
            return SortKey < Other.SortKey;
        }
    };

    struct FPointLightProxy
    {
        SIZE_T ProxyIndex;
    };
    
    struct FIndirectRenderBatch
    {
        CMaterial*  Material;

        
        FRHIBindingLayoutRef                    BindingLayout;
        FRHIBindingSetRef                       BindingSet;
                                                
        FRHIBufferRef                           IndirectDrawBuffer;
        FRHIBufferRef                           BatchToModelBuffer;

        THashMap<CStaticMesh*, int32>           MeshDrawIndex;

        TVector<uint32>                         DrawCallToModelIndexMap;
        TVector<FDrawIndexedIndirectArguments>  DrawIndexedArguments;
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
    };


    struct FSceneGlobalData
    {
        FCameraData    CameraData;
        float          Time;
        float          DeltaTime;
    };


}
