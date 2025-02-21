#pragma once

#include "Scene.h"
#include "Memory/RefCounted.h"
#include "ScenePrimitives.h"
#include "SceneRenderTypes.h"
#include "Renderer/Renderer.h"
#include "Renderer/Image.h"
#include "Renderer/RenderTypes.h"
#include "Renderer/Viewport.h"

namespace Lumina
{
    class FPrimitiveDrawManager;
    class AMaterial;
    struct FMaterialTexturesData;
    class FSceneUpdateContext;
    class AMaterialInstance;
    class FScene;
    class Material;
    class FRenderer;
    class FImageSampler;
    class AStaticMesh;
    class FBuffer;
    class FImage;
    class FCamera;
    
    struct FModelData
    {
        glm::mat4 ModelMatrix;
    };
    
    /**
     * Scene renderer's are stateful renderes that interface with the state-less renderer.
     * Responsible for managing all low-level scene rendering.
     */
    class FSceneRenderer
    {
    public:
        

        static FSceneRenderer* Create();

        FSceneRenderer();
        virtual ~FSceneRenderer();

        void Initialize();
        void Deinitialize();
        
        void StartScene(const FScene* Scene);
        void EndScene(const FScene* Scene);
        
        FRHIImage GetPrimaryRenderTarget() { return RenderTargets[FRenderer::GetFrameIndex()]; }
        FRHIImage GetDepthAttachment() { return DepthAttachments[FRenderer::GetFrameIndex()]; }

    protected:

        void ForwardRenderPass(const FScene* Scene);
        void FullScreenPass(const FScene* Scene);

        void DrawPrimitives(const FScene* Scene);

        void InitPipelines();
        void InitBuffers();
        void InitDescriptorSets();
        void CreateImages();
        void OnSwapchainResized();

        FORCEINLINE FSceneGlobalData* GetSceneGlobalData() { return &SceneGlobalData; }

        
    private:

        FViewport                           SceneViewport;
        TVector<FRHIDescriptorSet>          SceneGlobalDescriptorSets;
                                            
        TVector<FRHIImage>                  RenderTargets;
        TVector<FRHIImage>                  DepthAttachments;

                                            
        FRHIBuffer                          SceneGlobalUBO;
        FRHIBuffer                          LightSSBO;
        FRHIBuffer                          ModelSSBO;


        FSceneGlobalData                    SceneGlobalData;
        FSceneLightData                     SceneLightData;
        TVector<FModelData>                 ModelData;
        TVector<FMaterialTexturesData>      TexturesData;
    };
    
}
