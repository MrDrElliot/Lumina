#pragma once

#include "Scene.h"
#include "SceneRenderTypes.h"
#include "Entity/Components/StaticMeshComponent.h"
#include "Memory/SmartPtr.h"
#include "Renderer/DescriptorTableManager.h"
#include "Renderer/RenderResource.h"
#include "Renderer/RenderTypes.h"

namespace Lumina
{
    struct FStaticMeshRenderProxy;
    class CStaticMesh;
    class FPrimitiveDrawManager;
    struct FMaterialTexturesData;
    class FSceneUpdateContext;
    class AMaterialInstance;
    class FScene;
    class FRenderer;
    
    
    /**
     * Scene renderers are stateful renderers that interface with the state-less renderer.
     * Responsible for managing all low-level scene rendering.
     */
    class FSceneRenderer
    {
    public:
        
       
        FSceneRenderer(FScene* InScene);
        virtual ~FSceneRenderer();

        void Initialize();
        void Deinitialize();
        
        void Render(const FUpdateContext& UpdateContext);
    
        FRHIImageRef GetRenderTarget() const { return SceneViewport->GetRenderTarget(); }
        FSceneGlobalData* GetSceneGlobalData() { return &SceneGlobalData; }

        const FSceneRenderStats& GetSceneRenderStats() const { return SceneRenderStats; }
        const FGBuffer& GetGBuffer() const { return GBuffer; }
        FRHIImageRef GetDepthAttachment() const { return DepthAttachment; }
        FRHIImageRef GetSSAOImage() const { return SSAOImage; }
        
        ESceneRenderGBuffer GetGBufferDebugMode() const { return GBufferDebugMode; }
        void SetGBufferDebugMode(ESceneRenderGBuffer Mode) { GBufferDebugMode = Mode; }
    
    protected:

        FViewportState MakeViewportStateFromImage(const FRHIImage* Image);

        void OnStaticMeshComponentCreated();
        void OnStaticMeshComponentDestroyed();

        void BuildPasses();

        void InitResources();
        void InitBuffers();
        void CreateImages();
        void OnSwapchainResized();

        bool ResizeBufferIfNeeded(FRHIBufferRef& Buffer, SIZE_T DesiredSize) const;
    
    private:

        FScene*                             Scene = nullptr;
        IRenderContext*                     RenderContext = nullptr;
        FSceneRenderStats                   SceneRenderStats;
        
        FRHIViewportRef                     SceneViewport;
        
        TVector<FVertex>                    CombinedVertex;
        TVector<uint32>                     CombinedIndex;

        std::atomic<SIZE_T>                 NextVertexBufferWritePos = {0};
        FRHIBufferRef                       VertexBuffer;
        TVector<FVertex>                    Vertices;

        std::atomic<SIZE_T>                 NextIndexBufferWritePos = {0};
        FRHIBufferRef                       IndexBuffer;
        TVector<uint32>                     Indices;
        
        FRHIBufferRef                       SceneDataBuffer;
        FRHIBufferRef                       ModelDataBuffer;
        FRHIBufferRef                       LightDataBuffer;
        FRHIBufferRef                       SSAOKernalBuffer;
        
        FRHIInputLayoutRef                  VertexLayoutInput;
        
        FSceneGlobalData                    SceneGlobalData;

        FRHIBindingLayoutRef                SkyboxBindingLayout;
        FRHIBindingSetRef                   SkyboxBindingSet;
        FRHIBindingSetRef                   LightingPassSet;
        FRHIBindingLayoutRef                LightingPassLayout;
        
        FRHIBindingSetRef                   SSAOPassSet;
        FRHIBindingLayoutRef                SSAOPassLayout;

        FRHIBindingSetRef                   SSAOBlurPassSet;
        FRHIBindingLayoutRef                SSAOBlurPassLayout;
        
        FRHIBindingLayoutRef                BindingLayout;
        FRHIBindingSetRef                   BindingSet;
        
        
        FGBuffer                            GBuffer;
        
        FRHIImageRef                        DepthMap;
        FRHIImageRef                        DepthAttachment;
        FRHIImageRef                        CubeMap;
        FRHIImageRef                        IrradianceCube;
        FRHIImageRef                        ShadowCubeMap;
        FRHIImageRef                        NoiseImage;
        FRHIImageRef                        SSAOImage;
        FRHIImageRef                        SSAOBlur;

        ESceneRenderGBuffer                 GBufferDebugMode = ESceneRenderGBuffer::RenderTarget;

        FSceneLightData                     LightData;
        FModelData                          ModelData;

        TVector<FPointLightProxy>           PointLightProxies;

        TVector<FStaticMeshRender>              StaticMeshRenders;
        TVector<FIndirectRenderBatch>           RenderBatches;
        TVector<FDrawIndexedIndirectArguments>  IndirectDrawArguments;

        FRHIBufferRef                       IndirectDrawBuffer;
        THashMap<CStaticMesh*, SIZE_T>      MeshVertexOffset;
        THashMap<CStaticMesh*, SIZE_T>      MeshIndexOffset;
        TSet<CStaticMesh*>                  RegisteredMeshes;
        TSet<CMaterial*>                    RegisteredMaterials;

        TVector<FStaticMeshRenderProxy*> StaticMeshProxies;
        
        TSharedPtr<FDescriptorTableManager> DescriptorTableManager;
    };
    
}
