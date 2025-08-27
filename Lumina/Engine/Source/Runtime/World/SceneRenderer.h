#pragma once

#include "SceneRenderTypes.h"
#include "Containers/SparseArray.h"
#include "Renderer/RenderResource.h"
#include "Renderer/RenderTypes.h"
#include "Entity/Components/RenderComponent.h"

namespace Lumina
{
    struct SDirectionalLightComponent;
}

namespace Lumina
{
    struct SEnvironmentComponent;
}

namespace Lumina
{
    class FRenderGraph;
    class CWorld;
    class FUpdateContext;
    class CStaticMesh;
    class FPrimitiveDrawManager;
    struct FMaterialTexturesData;
    class CMaterialInstance;
    class FRenderer;

    template<typename T> using TRenderVector = TFixedVector<T, 2024>;
    
    class FSceneRenderer
    {
    public:
        
        friend struct SRenderComponent;
       
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
        
        void AddEnvironmentComponent(entt::entity entity);
        void RemoveEnvironmentComponent(entt::entity entity);
        void AddDirectionalLightComponent(entt::entity entity);
        void RemoveDirectionalLightcomponent(entt::entity entity);
        
        FViewportState MakeViewportStateFromImage(const FRHIImage* Image);

        void BuildPasses();
        void BuildDrawCalls();

        void InitResources();
        void InitBuffers();
        void CreateImages();
        void OnSwapchainResized();
    

    private:

        CWorld*                             World = nullptr;
        SEnvironmentComponent*              EnvironmentComponent = nullptr;
        SDirectionalLightComponent*         DirectionalLightComponent = nullptr;
        
        FSceneRenderStats                   SceneRenderStats;
        FSceneRenderSettings                RenderSettings;
        FSceneLightData                     LightData;

        FRHIViewportRef                     SceneViewport;

        TRenderVector<FSimpleElementVertex>  SimpleVertices;
        FRHIBindingLayoutRef                SimplePassLayout;

        
        FRHIBufferRef                       SimpleVertexBuffer;
        FRHIBufferRef                       SceneDataBuffer;
        FRHIBufferRef                       EnvironmentBuffer;
        FRHIBufferRef                       ModelDataBuffer;
        FRHIBufferRef                       LightDataBuffer;
        FRHIBufferRef                       SSAOKernalBuffer;
        FRHIBufferRef                       SSAOSettingsBuffer;
        FRHIBufferRef                       IndirectDrawBuffer;

        FRHIInputLayoutRef                  VertexLayoutInput;
        FRHIInputLayoutRef                  SimpleVertexLayoutInput;

        FSceneGlobalData                    SceneGlobalData;

        FRHIBindingLayoutRef                EnvironmentLayout;
        FRHIBindingSetRef                   EnvironmentBindingSet;
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

        TRenderVector<FInstanceData>                  InstanceData;
        
        TRenderVector<FPointLightProxy>               PointLightProxies;

        TRenderVector<FStaticMeshRender>              StaticMeshRenders;
        TRenderVector<FIndirectRenderBatch>           RenderBatches;
        TRenderVector<FDrawIndexedIndirectArguments>  IndirectDrawArguments;

    };
    
}
