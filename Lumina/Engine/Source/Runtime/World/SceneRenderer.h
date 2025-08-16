#pragma once

#include "SceneRenderTypes.h"
#include "Entity/Components/StaticMeshComponent.h"
#include "Memory/SmartPtr.h"
#include "Renderer/DescriptorTableManager.h"
#include "Renderer/RenderResource.h"
#include "Renderer/RenderTypes.h"

namespace Lumina
{
    class CWorld;
    class FUpdateContext;
    struct FStaticMeshRenderProxy;
    class CStaticMesh;
    class FPrimitiveDrawManager;
    struct FMaterialTexturesData;
    class AMaterialInstance;
    class FRenderer;
    
    
    class FSceneRenderer
    {
    public:
        
       
        FSceneRenderer(CWorld* InWorld);
        virtual ~FSceneRenderer();
        
        void Render(const FUpdateContext& UpdateContext);
    
        FRHIImageRef GetRenderTarget() const { return SceneViewport->GetRenderTarget(); }
        FSceneGlobalData* GetSceneGlobalData() { return &SceneGlobalData; }

        FSceneRenderSettings& GetSceneRenderSettings() { return RenderSettings; }
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
        void BuildDrawCalls();

        void InitResources();
        void InitBuffers();
        void CreateImages();
        void OnSwapchainResized();

        bool ResizeBufferIfNeeded(FRHIBufferRef& Buffer, SIZE_T DesiredSize) const;
    
    private:

        CWorld*                             World = nullptr;
        FSceneRenderStats                   SceneRenderStats;
        FSceneRenderSettings                RenderSettings;
        
        FRHIViewportRef                     SceneViewport;
        
        TVector<FVertex>                    CombinedVertex;
        TVector<uint32>                     CombinedIndex;

        std::atomic<SIZE_T>                 NextVertexBufferWritePos = {0};
        FRHIBufferRef                       VertexBuffer;
        TVector<FVertex>                    Vertices;

        std::atomic<SIZE_T>                 NextIndexBufferWritePos = {0};
        FRHIBufferRef                       IndexBuffer;
        TVector<uint32>                     Indices;

        TVector<FSimpleElementVertex>       SimpleVertices;
        FRHIBindingLayoutRef                SimplePassLayout;

        
        FRHIBufferRef                       SimpleVertexBuffer;
        FRHIBufferRef                       SceneDataBuffer;
        FRHIBufferRef                       ModelDataBuffer;
        FRHIBufferRef                       LightDataBuffer;
        FRHIBufferRef                       SSAOKernalBuffer;
        FRHIBufferRef                       SSAOSettingsBuffer;

        FRHIInputLayoutRef                  VertexLayoutInput;
        FRHIInputLayoutRef                  SimpleVertexLayoutInput;

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

        TVector<FStaticMeshRenderProxy*>    StaticMeshProxies;
        
        TSharedPtr<FDescriptorTableManager> DescriptorTableManager;
    };
    
}
