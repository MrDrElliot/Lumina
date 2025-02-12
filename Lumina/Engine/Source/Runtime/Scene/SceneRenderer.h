#pragma once

#include "Memory/RefCounted.h"
#include "ScenePrimitives.h"
#include "Assets/AssetHandle.h"
#include "Renderer/Renderer.h"
#include "Renderer/Image.h"
#include "Renderer/RenderTypes.h"


#define MAX_LIGHTS 102

namespace Lumina
{
    class FSceneUpdateContext;
}

namespace Lumina
{
    struct FMaterialTexturesData;
    class AMaterialInstance;
    class FScene;
    class Material;
    class FRenderer;
    class FDescriptorSet;
    class FImageSampler;
    class AStaticMesh;
    class FBuffer;
    class FImage;
    class FCamera;
    
    struct FModelData
    {
        glm::mat4 ModelMatrix;
    };

    struct FMaterialUniforms
    {
        FMaterialUniforms()
        {
            memset(padding, 0, sizeof(padding));
        }
        
        glm::vec4 baseColor =    glm::vec4(1.0f);   // 16 bytes, already aligned
        float roughness =          0.5f;                 // 4 bytes
        float metallic =           0.5f;                 // 4 bytes
        float padding[2];                                // 8 bytes padding to align to 16 bytes
    };

    struct FLight
    {
        glm::vec4 LightPosition =   glm::vec4(0.0f);
        glm::vec4 LightColor =      glm::vec4(0.0f);
    };

    struct FSceneLightData
    {
        FSceneLightData()
        {
            memset(Lights, 0, sizeof(Lights));
            memset(padding, 0, sizeof(padding));
        }
        
        uint32 NumLights =  0;
        uint32 padding      [3];
        FLight Lights       [MAX_LIGHTS];
    };
    
    /**
     * Scene renderer's are stateful renderes that interface with the state-less renderer.
     * Responsible for managing all low-level scene rendering.
     */
    class FSceneRenderer : public ISubsystem
    {
    public:
        

        static FSceneRenderer* Create(FScene* InScene);

        FSceneRenderer(FScene* InScene);
        ~FSceneRenderer();

        void Initialize(const FSubsystemManager& Manager) override;
        void Deinitialize() override;
        
        void StartFrame();
        void EndFrame();

        TRefPtr<FImage> GetPrimaryRenderTarget() { return RenderTargets[FRenderer::GetCurrentFrameIndex()]; }
        TRefPtr<FImage> GetDepthAttachment() { return DepthAttachments[FRenderer::GetCurrentFrameIndex()]; }
        FSceneLightData& GetSceneLightingData() { return SceneLightingData; }

        void RenderGrid();
        void GeometryPass(const FSceneUpdateContext& SceneContext);

        void InitPipelines();
        void InitBuffers();
        void InitDescriptorSets();
        void CreateImages();
        void Shutdown();
        void OnSwapchainResized();

    private:
        
        TRefPtr<FPipeline> GraphicsPipeline;
        TRefPtr<FPipeline> InfiniteGridPipeline;
        
        TVector<TRefPtr<FDescriptorSet>> GridDescriptorSets;
        TVector<TRefPtr<FDescriptorSet>> SceneDescriptorSets;
        
        FMaterialAttributes Attributes;
        
        struct FTransientData
        {
            uint32 ModelIndex = 0;       // 4 bytes
            uint32 MaterialIndex = 1;    // 4 bytes
            uint32 Padding[2];           // 8 bytes padding to make the struct 16 bytes aligned
        } Data;

        
        TVector<TRefPtr<FImage>> RenderTargets;
        TVector<TRefPtr<FImage>> DepthAttachments;

        
        TRefPtr<FImage> BaseColor;
        TRefPtr<FImage> Emissive;
        TRefPtr<FImage> Normal;
        TRefPtr<FImage> Metallic;
        TRefPtr<FImage> AmbientOcclusion;
        
        
        TRefPtr<FBuffer> SceneUBO;
        TRefPtr<FBuffer> ModelSBO;
        TRefPtr<FBuffer> CameraUBO;
        TRefPtr<FBuffer> MaterialUBO;


        FCameraData                         CameraData;
        FSceneLightData                     SceneLightingData;
        TVector<FModelData>                 ModelData;
        TVector<FMaterialTexturesData>      TexturesData;
        
        TRefPtr<FMaterial> TestMaterial;
        
        FScene* CurrentScene = nullptr;
        
    };
}
