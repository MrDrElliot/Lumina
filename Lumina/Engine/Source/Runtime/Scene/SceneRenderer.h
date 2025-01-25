#pragma once
#include <memory>

#include "ScenePrimitives.h"
#include "Assets/AssetHandle.h"
#include "Renderer/Renderer.h"
#include "Renderer/Image.h"


#define MAX_LIGHTS 102

namespace Lumina
{
    class LMaterialInstance;
    class LScene;
    class Material;
    class FRenderer;
    class FDescriptorSet;
    class FImageSampler;
    class LStaticMesh;
    class FBuffer;
    class FImage;
    class FCamera;
    
    struct FModelData
    {
        glm::mat4 model;
    };

    struct FMaterialUniforms
    {
        glm::vec4 baseColor;    // 16 bytes, already aligned
        float roughness;        // 4 bytes
        float metallic;         // 4 bytes
        float padding[2];       // 8 bytes padding to align to 16 bytes
    };

    struct FSceneLightData
    {
        uint32 NumLights = 0;
        uint32 padding[3];
        glm::vec4 LightPosition [MAX_LIGHTS];
        glm::vec4 LightColor    [MAX_LIGHTS];
    };

    struct FGridData
    {
        glm::mat4 View =         glm::mat4(1.0f);
        glm::mat4 Projection =   glm::mat4(1.0f);
        glm::vec3 Position =     {0.0f, 0.0f, 0.0f};
    };
    
    class FSceneRenderer
    {
    public:
        

        static std::shared_ptr<FSceneRenderer> Create(LScene* InScene);

        FSceneRenderer(LScene* InScene);
        ~FSceneRenderer();

        void BeginScene(std::shared_ptr<FCamera> InCamera);
        void EndScene();

        TRefPtr<FImage> GetRenderTarget() { return      RenderTargets[FRenderer::GetCurrentFrameIndex()]; }
        TRefPtr<FImage> GetDepthAttachment() { return   DepthAttachments[FRenderer::GetCurrentFrameIndex()]; }
        FSceneLightData& GetSceneLightingData() { return SceneLightingData; }

        void RenderGrid();
        void GeometryPass(const TFastVector<TRefPtr<FImage>>& Attachments);

        void InitPipelines();
        void InitBuffers();
        void InitDescriptorSets();
        void CreateImages();
        void Shutdown();
        void OnSwapchainResized();

    private:
        
        TRefPtr<FPipeline> GraphicsPipeline;
        TRefPtr<FPipeline> InfiniteGridPipeline;
        
        TFastVector<TRefPtr<FDescriptorSet>> GridDescriptorSets;
        TFastVector<TRefPtr<FDescriptorSet>> SceneDescriptorSets;

        FMaterialAttributes Attributes;
        
        struct FTransientData
        {
            uint32 ModelIndex = 0;       // 4 bytes
            uint32 MaterialIndex = 1;    // 4 bytes
            uint32 Padding[2];           // 8 bytes padding to make the struct 16 bytes aligned
        } Data;

        
        TFastVector<TRefPtr<FImage>> RenderTargets;
        TFastVector<TRefPtr<FImage>> DepthAttachments;

        TRefPtr<FImage> BaseColor;
        TRefPtr<FImage> Emissive;
        TRefPtr<FImage> Normal;
        TRefPtr<FImage> Metallic;
        TRefPtr<FImage> AmbientOcclusion;
        
        
        TRefPtr<FBuffer> SceneUBO;
        TRefPtr<FBuffer> ModelSBO;
        TRefPtr<FBuffer> CameraUBO;
        TRefPtr<FBuffer> GridUBO;
        TRefPtr<FBuffer> MaterialUBO;


        FGridData                           GridData;
        FCameraData                         CameraData;
        FSceneLightData                     SceneLightingData;
        TFastVector<FModelData>             ModelData;
        TFastVector<FMaterialTexturesData>  TexturesData;

        TAssetHandle<LMaterialInstance> MaterialInstance;

        
        std::shared_ptr<FCamera> Camera;
        TRefPtr<FMaterial> TestMaterial;
        
        LScene* CurrentScene;

        uint32 MSAASamples = 4;
        
    };
}
