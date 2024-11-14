#pragma once
#include <memory>

#include "ScenePrimitives.h"
#include "Renderer/Renderer.h"
#include "Renderer/Image.h"

namespace Lumina
{
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

    struct FLightData
    {
        glm::vec4 lightPosition =   {0.0f, 0.0f, 0.0f, 0.0f};
        glm::vec4 cameraPosition =  {0.0f, 0.0f, 0.0f, 0.0f};
        glm::vec4 lightColor =      {1.0f, 1.0f, 1.0f, 1.0f};
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

        TRefPtr<FImage> GetRenderTarget() { return RenderTargets[FRenderer::GetCurrentFrameIndex()]; }
        TRefPtr<FImage> GetDepthAttachment() { return DepthAttachments[FRenderer::GetCurrentFrameIndex()]; }
        FLightData& GetSceneLightingData() { return SceneLightingData; }

        void RenderGrid();
        void GeometryPass(const TFastVector<TRefPtr<FImage>>& Attachments);
        void TAAPass();

        void InitPipelines();
        void InitBuffers();
        void InitDescriptorSets();
        void CreateImages();
        void Shutdown();

    private:
        
        TRefPtr<FPipeline> GraphicsPipeline;
        TRefPtr<FPipeline> InfiniteGridPipeline;
        TRefPtr<FPipeline> TAAPipeline;
        
        TFastVector<TRefPtr<FDescriptorSet>> GridDescriptorSets;
        TFastVector<TRefPtr<FDescriptorSet>> SceneDescriptorSets;
        TFastVector<TRefPtr<FDescriptorSet>> TAADescriptorSets;

        
        TFastVector<TRefPtr<FImage>> RenderTargets;
        TFastVector<TRefPtr<FImage>> DepthAttachments;
        TFastVector<TRefPtr<FImage>> HistoryRenderTarget;
        TFastVector<TRefPtr<FImage>> MotionVectors;

        
        TRefPtr<FBuffer> SceneUBO;
        TRefPtr<FBuffer> ModelSBO;
        TRefPtr<FBuffer> CameraUBO;
        TRefPtr<FBuffer> GridUBO;

        FGridData GridData;
        FCameraData CameraData;
        FLightData SceneLightingData;
        TFastVector<FModelData> ModelData;

        std::shared_ptr<FCamera> Camera;
        LScene* CurrentScene;

        uint32 MSAASamples = 4;
        
    };
}
