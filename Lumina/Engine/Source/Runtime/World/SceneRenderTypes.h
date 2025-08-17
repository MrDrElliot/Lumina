#pragma once

#include <glm/glm.hpp>

#include "Core/Math/Transform.h"
#include "Platform/GenericPlatform.h"
#include "Renderer/MeshData.h"
#include "Renderer/RenderResource.h"
#include "Renderer/RHIFwd.h"

#define MAX_LIGHTS 1024
#define MAX_MODELS 10000

namespace Lumina
{
    class CMaterialInterface;
}

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
        Depth,
        SSAO,
    };
    
    struct FCameraData
    {
        glm::vec4 Location =    {};
        glm::mat4 View =        {};
        glm::mat4 InverseView = {};
        glm::mat4 Projection =  {};
        glm::mat4 InverseProjection = {};
    };
    
    constexpr uint32 LIGHT_TYPE_DIRECTIONAL = 0;
    constexpr uint32 LIGHT_TYPE_POINT       = 1;
    constexpr uint32 LIGHT_TYPE_SPOT        = 2;

    struct FLight
    {
        glm::vec4 Position      = glm::vec4(0.0f);
        glm::vec4 Direction     = glm::vec4(0.0f);
        glm::vec4 Color         = glm::vec4(0.0f);
        glm::vec2 Angle         = glm::vec2(10.0f);
        float Radius            = 10.0f;
        uint32 Type             = 0;
    };
    
    struct FSceneLightData
    {
        uint32 NumLights = 0;
        uint32 Padding[3] = {};
        FLight Lights[MAX_LIGHTS];
    };

    struct FSSAOSettings
    {
        float Radius = 1.0f;
        float Intensity = 2.0f;
        float Power = 1.5f;
    };

    struct FModelData
    {
        TVector<glm::mat4> ModelMatrices;
    };

    struct FSceneRegisteredMeshAsset
    {
        CStaticMesh* Mesh;
        SIZE_T VertexOffset;
        SIZE_T IndexOffset;
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
        CMaterialInterface* Material = nullptr;
        CStaticMesh*        StaticMesh = nullptr;
        uint32              NumDraws;
        uint32              Offset;
    };

    struct FStaticMeshRender
    {
        CMaterialInterface* Material = nullptr;
        CStaticMesh*        StaticMesh = nullptr;
        FGeometrySurface    Surface;
        glm::mat4           Matrix;
        uint32              VertexOffset;
        uint32              FirstIndex;
        SIZE_T              SortKey;

        bool operator < (const FStaticMeshRender& Other) const
        {
            return SortKey < Other.SortKey;
        }
        
    };

    struct FPointLightProxy
    {
        SIZE_T ProxyIndex;
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
        float          NearPlane;
        float          FarPlane;
    };

    struct FSceneRenderSettings
    {
        bool bDrawAABB = false;
        bool bSSAO = false;
        FSSAOSettings SSAOSettings;
    };
}

namespace eastl
{
    template <>
    struct hash<FSceneRegisteredMeshAsset>
    {
        size_t operator()(const FSceneRegisteredMeshAsset& Asset) const noexcept
        {
            return eastl::hash<void*>{}(Asset.Mesh);
        }
    };
}
