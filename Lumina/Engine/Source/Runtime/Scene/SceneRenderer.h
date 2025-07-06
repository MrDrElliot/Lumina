#pragma once

#include "Scene.h"
#include "ScenePrimitives.h"
#include "SceneRenderTypes.h"
#include "Renderer/RenderResource.h"
#include "Renderer/RenderTypes.h"
#include "Renderer/RenderGraph/RenderGraph.h"

namespace Lumina
{
    class CStaticMesh;
    struct FStaticMeshComponent;
    class FPrimitiveDrawManager;
    struct FMaterialTexturesData;
    class FSceneUpdateContext;
    class AMaterialInstance;
    class FScene;
    class Material;
    class FRenderer;
    class AStaticMesh;
    class FBuffer;
    class FCamera;
    
    
    /**
     * Scene renderers are stateful renderers that interface with the state-less renderer.
     * Responsible for managing all low-level scene rendering.
     */
    class FSceneRenderer
    {
    public:
        
       
        FSceneRenderer();
        virtual ~FSceneRenderer();

        void Initialize();
        void Deinitialize();
        
        void StartScene(const FScene* Scene);
        void EndScene(const FScene* Scene);
    
        INLINE FRHIImageRef GetRenderTarget() const { return SceneViewport->GetRenderTarget(); }
        INLINE FSceneGlobalData* GetSceneGlobalData() { return &SceneGlobalData; }

    protected:

        void GeometryPass(const FSceneRenderData& DrawList);
        void LightingPass(const FSceneRenderData& DrawList);
        void SkyboxPass(const FSceneRenderData& DrawList);
        void DrawPrimitives(const FSceneRenderData& DrawList);


        void InitResources();
        void InitBuffers();
        void CreateImages();
        void OnSwapchainResized();


        void FullScreenPass(const FScene* Scene);
        
        
    private:

        IRenderContext*                     RenderContext = nullptr;
        FRHIImageRef                        CubeMap;
        FRHIViewportRef                     SceneViewport;
        FRHIBufferRef                       SceneDataBuffer;
        FRHIBufferRef                       ModelDataBuffer;
        FRHIBufferRef                       SceneLightBuffer;
        FRHIInputLayoutRef                  VertexLayoutInput;
        
        FSceneGlobalData                    SceneGlobalData;
        TVector<FModelData>                 ModelData;
        TVector<FMaterialTexturesData>      TexturesData;

        FRHIBindingSetRef                   SceneGlobalBindingSet;
        FRHIBindingLayoutRef                SceneGlobalBindingLayout;
        
        FRHIBindingLayoutRef                SimpleBindingLayout;
        FRHIBindingSetRef                   SimpleBindingLayoutSet;
        
        FRenderGraph                        RenderGraph;
        FGBuffer                            GBuffer;
        FRHIImageRef                        DepthAttachment;
    };
    
}
