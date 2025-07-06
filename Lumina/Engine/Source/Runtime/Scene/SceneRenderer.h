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
    
    struct FModelData
    {
        glm::mat4 ModelMatrix;
    };

    struct FGBuffer
    {
        FRHIImageRef Position;
        FRHIImageRef Normals;
        FRHIImageRef AlbedoSpec;
    };

    struct FFrameDrawList
    {
        TVector<CStaticMesh*> Meshes;
        TVector<FModelData> ModelData;
    };
    
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

        void GeometryPass(const FFrameDrawList& DrawList);
        void LightingPass(const FFrameDrawList& DrawList);
        void SkyboxPass(const FFrameDrawList& DrawList);
        void FullScreenPass(const FScene* Scene);

        void DrawPrimitives(const FScene* Scene);

        void InitResources();
        void InitBuffers();
        void CreateImages();
        void OnSwapchainResized();


        
        
    private:

        IRenderContext*                     RenderContext = nullptr;
        FRHIImageRef                        CubeMap;
        FRHIViewportRef                     SceneViewport;
        FRHIBufferRef                       SceneDataBuffer;
        FRHIBufferRef                       ModelDataBuffer;
        FRHIInputLayoutRef                  VertexLayoutInput;
        
        FSceneGlobalData                    SceneGlobalData;
        FSceneLightData                     SceneLightData;
        TVector<FModelData>                 ModelData;
        TVector<FMaterialTexturesData>      TexturesData;

        FRHIBindingSetRef                   SceneGlobalBindingSet;
        FRHIBindingLayoutRef                SceneGlobalBindingLayout;
        
        FRenderGraph                        RenderGraph;
        FGBuffer                            GBuffer;
        FRHIImageRef                        DepthAttachment;
    };
    
}
