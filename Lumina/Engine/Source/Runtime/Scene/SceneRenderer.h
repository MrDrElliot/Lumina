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

        void GeometryPass(const FSceneRenderData& DrawList);
        void LightingPass(const FSceneRenderData& DrawList);
        void SkyboxPass(const FSceneRenderData& DrawList);
        void DrawPrimitives(const FSceneRenderData& DrawList);

        void CreateIrradianceCube();

        void InitResources();
        void InitBuffers();
        void CreateImages();
        void OnSwapchainResized();

        void CreateOrResizeGeometryBuffers(uint64 VertexSize, uint64 IndexSize, uint64 VertexSizeDesired, uint64 IndexSizeDesired);
        void CreateOrResizeDrawCommandBuffer(uint64 SizeRequired, uint64 SizeDesired);

        void FullScreenPass(const FScene* Scene);
        
        
    private:

        FScene*                             Scene = nullptr;
        IRenderContext*                     RenderContext = nullptr;
        FSceneRenderStats                   SceneRenderStats;
        
        FRHIViewportRef                     SceneViewport;
        
        TVector<FVertex>                    CombinedVertex;
        TVector<uint32>                     CombinedIndex;
        
        FRHIBufferRef                       VertexBuffer;
        FRHIBufferRef                       IndexBuffer;
        FRHIBufferRef                       DrawCommandBuffer;
        
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

        FSceneRenderData                    LastFrameRenderData;
        ESceneRenderGBuffer                 GBufferDebugMode = ESceneRenderGBuffer::RenderTarget;

        TVector<FRenderProxy>               RenderProxies;
        TVector<FRenderProxy>               SortedRenderProxies;
        TVector<FIndirectRenderBatch>       RenderBatch;
        THashMap<SIZE_T, SIZE_T>            RenderBatchMap;

        FModelData                              ModelData;
        THashMap<uint64, SIZE_T>                IndirectArgsMap;
        TVector<FDrawIndexedIndirectArguments>  DrawIndexedArguments;

        
        SIZE_T                              NumMeshBlocks;
        THashMap<CStaticMesh*, SIZE_T>      MeshBlocks;
        TSet<CStaticMesh*>                  RegisteredMeshes;
        
        TSharedPtr<FDescriptorTableManager> DescriptorTableManager;
    };
    
}
