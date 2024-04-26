#pragma once
#include <memory>
#include <glm/glm.hpp>

#include "ScenePrimitives.h"
#include "Source/Runtime/Renderer/Renderer.h"
#include "Source/Runtime/Renderer/RenderTypes.h"


namespace Lumina
{
    class FRenderer;
    class FDescriptorSet;
    class FImageSampler;
    class LStaticMesh;
    class FBuffer;
    class FImage;
    class FCamera;

    class FSceneRenderer
    {
    public:

        static std::shared_ptr<FSceneRenderer> Create();

        FSceneRenderer();
        ~FSceneRenderer();

        void BeginScene(std::shared_ptr<FCamera> InCamera);
        void EndScene();

        std::shared_ptr<FImage> GetRenderTarget() { return RenderTargets[FRenderer::GetCurrentFrameIndex()]; }
        std::shared_ptr<FImage> GetDepthAttachment() { return DepthAttachments[FRenderer::GetCurrentFrameIndex()]; }
        std::shared_ptr<FImageSampler> GetNearestSampler() { return NearestSampler; }
        std::shared_ptr<FImageSampler> GetLinearSampler() { return LinearSampler; }

        void InitPipelines();
        void InitBuffers();
        void InitDescriptorSets();
        void CreateImages();
        void Shutdown();

    private:

        float angle = 0.0f;  // Initial angle
        float rotationSpeed = 0.01f;  // Speed of rotation (radians per update)

        FMiscData TransformData;

        FGBuffer GBuffer;
        
        std::vector<std::shared_ptr<FDescriptorSet>> SceneDescriptorSets;
        
        std::vector<std::shared_ptr<FImage>> RenderTargets;
        std::vector<std::shared_ptr<FImage>> DepthAttachments;

        std::shared_ptr<FImage> RandomTexture;

        std::shared_ptr<FImageSampler> NearestSampler;
        std::shared_ptr<FImageSampler> LinearSampler;

        std::vector<std::shared_ptr<LStaticMesh>> RenderMeshes;
        
        std::shared_ptr<FBuffer> CameraBuffer;
        std::shared_ptr<FBuffer> TransformBuffer;
        std::shared_ptr<FBuffer> TextureBuffer;

        std::shared_ptr<FCamera> Camera;
    };
}
