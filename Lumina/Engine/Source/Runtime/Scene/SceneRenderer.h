#pragma once

#include "Scene.h"
#include "SceneRenderTypes.h"
#include "Memory/SmartPtr.h"
#include "Renderer/DescriptorTableManager.h"
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
        
       
        FSceneRenderer(FScene* InScene);
        virtual ~FSceneRenderer();

        void Initialize();
        void Deinitialize();
        
        void StartScene(const FUpdateContext& UpdateContext);
        void EndScene(const FUpdateContext& UpdateContext);
    
        INLINE FRHIImageRef GetRenderTarget() const { return SceneViewport->GetRenderTarget(); }
        INLINE FSceneGlobalData* GetSceneGlobalData() { return &SceneGlobalData; }

        const FSceneRenderStats& GetSceneRenderStats() const { return SceneRenderStats; }
        const FGBuffer& GetGBuffer() const { return GBuffer; }
        
        ESceneRenderGBuffer GetGBufferDebugMode() const { return GBufferDebugMode; }
        void SetGBufferDebugMode(ESceneRenderGBuffer Mode) { GBufferDebugMode = Mode; }
        
    protected:

        void GeometryPass();
        void LightingPass();
        void SkyboxPass();
        void DrawPrimitives();

        void CreateIrradianceCube();

        void InitResources();
        void InitBuffers();
        void CreateImages();
        void OnSwapchainResized();

        void CreateOrResizeGeometryBuffers(uint64 VertexSize, uint64 IndexSize, uint64 VertexSizeDesired, uint64 IndexSizeDesired);

        void FullScreenPass(const FScene* Scene);

    private:

        void UpdateGeometryBuffersForNewMeshes(ICommandList* CommandList, const TVector<CStaticMesh*>& NewMeshes);
        
    private:

        FScene*                             Scene = nullptr;
        IRenderContext*                     RenderContext = nullptr;
        FSceneRenderStats                   SceneRenderStats;
        
        FRHIViewportRef                     SceneViewport;
        
        TVector<FVertex>                    CombinedVertex;
        TVector<uint32>                     CombinedIndex;
        
        FRHIBufferRef                       VertexBuffer;
        FRHIBufferRef                       IndexBuffer;
        
        FRHIBufferRef                       SceneDataBuffer;
        FRHIBufferRef                       ModelDataBuffer;
        FRHIBufferRef                       LightDataBuffer;
        FRHIInputLayoutRef                  VertexLayoutInput;
        
        FSceneGlobalData                    SceneGlobalData;

        FRHIBindingLayoutRef                SkyboxBindingLayout;
        FRHIBindingSetRef                   SkyboxBindingSet;
        FRHIBindingSetRef                   LightingPassSet;
        FRHIBindingLayoutRef                LightingPassLayout;
        FRHIBindingLayoutRef                BindingLayout;
        FRHIBindingSetRef                   BindingSet;
        
        FRenderGraph                        RenderGraph;
        FGBuffer                            GBuffer;
        
        FRHIImageRef                        DepthAttachment;
        FRHIImageRef                        CubeMap;
        FRHIImageRef                        IrradianceCube;

        ESceneRenderGBuffer                 GBufferDebugMode = ESceneRenderGBuffer::RenderTarget;

        FSceneLightData                     LightData;
        FModelData                          ModelData;

        TVector<FPointLightProxy>           PointLightProxies;
        
        TVector<FRenderProxy>               RenderProxies;
        TVector<FRenderProxy>               SortedRenderProxies;

        THashMap<SIZE_T, SIZE_T>            IndirectBatchMap;
        TVector<FIndirectRenderBatch>       IndirectRenderBatch;

        TSet<CStaticMesh*>                  RegisteredMeshes;
        TSet<CMaterial*>                    RegisteredMaterials;
        THashMap<CStaticMesh*, SIZE_T>      VertexStartMap;
        THashMap<CStaticMesh*, SIZE_T>      IndexStartMap;
        
        
        TSharedPtr<FDescriptorTableManager> DescriptorTableManager;
    };
    
}
