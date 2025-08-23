#include "SceneRenderer.h"

#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Core/Windows/Window.h"
#include "Entity/Components/CameraComponent.h"
#include "Entity/Entity.h"
#include "Renderer/RHIIncl.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <execution>

#include "World.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "glm/gtx/quaternion.hpp"
#include "Core/Engine/Engine.h"
#include "Core/Profiler/Profile.h"
#include "Entity/Components/EditorComponent.h"
#include "entity/components/EnvironmentComponent.h"
#include "Entity/Components/LightComponent.h"
#include "Entity/Components/LineBatcherComponent.h"
#include "Entity/Components/StaticMeshComponent.h"
#include "Paths/Paths.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"
#include "Renderer/RHIStaticStates.h"
#include "Renderer/ShaderCompiler.h"
#include "Renderer/RenderGraph/RenderGraph.h"
#include "Renderer/RenderGraph/RenderGraphDescriptor.h"
#include "Subsystems/FCameraManager.h"
#include "TaskSystem/TaskSystem.h"
#include "Tools/Import/ImportHelpers.h"

namespace Lumina
{
    
    FSceneRenderer::FSceneRenderer(CWorld* InWorld)
        : World(InWorld)
        , SceneRenderStats()
        , SceneGlobalData()
    {
        SceneViewport = GRenderContext->CreateViewport(Windowing::GetPrimaryWindowHandle()->GetExtent());
        
        LOG_TRACE("Initializing Scene Renderer");
        
        // Wait for shader tasks.
        while (GRenderContext->GetShaderCompiler()->HasPendingRequests()) {}
        
        InitBuffers();
        CreateImages();
        InitResources();
    }

    FSceneRenderer::~FSceneRenderer()
    {
        GRenderContext->WaitIdle();
        
        LOG_TRACE("Shutting down scene renderer");
    }
    
    void FSceneRenderer::Render(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();
        SceneRenderStats = {};

        ICommandList* GraphicsCommandList = GRenderContext->GetCommandList(Q_Graphics);
        
        SCameraComponent& CameraComponent = World->GetActiveCamera();

        SceneGlobalData.CameraData.Location =           glm::vec4(CameraComponent.GetPosition(), 1.0f);
        SceneGlobalData.CameraData.View =               CameraComponent.GetViewMatrix();
        SceneGlobalData.CameraData.InverseView =        CameraComponent.GetViewVolume().GetInverseViewMatrix();
        SceneGlobalData.CameraData.Projection =         CameraComponent.GetProjectionMatrix();
        SceneGlobalData.CameraData.InverseProjection =  CameraComponent.GetViewVolume().GetInverseProjectionMatrix();
        SceneGlobalData.Time =                          (float)World->GetTimeSinceSceneCreation();
        SceneGlobalData.DeltaTime =                     (float)World->GetSceneDeltaTime();
        SceneGlobalData.FarPlane =                      1000.0f;
        SceneGlobalData.NearPlane =                     0.01f;
        
        SceneViewport->SetViewVolume(CameraComponent.GetViewVolume());
        
        FRenderGraph RenderGraph;
        
        BuildPasses();
        
        RenderGraph.AddPass<RG_Transfer>(FRGEvent("Write Scene Buffer"), nullptr, [&](ICommandList& CmdList)
        {
            LUMINA_PROFILE_SECTION_COLORED("Write Scene Buffer", tracy::Color::Orange4);

            GraphicsCommandList->WriteBuffer(SceneDataBuffer, &SceneGlobalData, 0, sizeof(FSceneGlobalData));
        });
        
        RenderGraph.AddPass<RG_Raster>(FRGEvent("Pre-Depth Pass"), nullptr, [&] (ICommandList& CmdList)
        {
            LUMINA_PROFILE_SECTION_COLORED("Pre-Depth Pass", tracy::Color::Orange);
            
            FRHIVertexShaderRef VertexShader = GRenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("DepthPrePass.vert");
            if (!VertexShader)
            {
                return;
            }
            
            for (SIZE_T CurrentDraw = 0; CurrentDraw < RenderBatches.size(); ++CurrentDraw)
            {
                const FIndirectRenderBatch& Batch = RenderBatches[CurrentDraw];

                FGraphicsState GraphicsState;
            
                FVertexBufferBinding VertexBufferBinding;
                VertexBufferBinding.SetBuffer(Batch.StaticMesh->GetMeshResource().VertexBuffer);
                
                FIndexBufferBinding IndexBufferBinding;
                IndexBufferBinding.SetBuffer(Batch.StaticMesh->GetMeshResource().IndexBuffer);

                
                GraphicsState.AddVertexBuffer(VertexBufferBinding);
                GraphicsState.SetIndexBuffer(IndexBufferBinding);
                
                FRenderPassBeginInfo BeginInfo; BeginInfo
                    .SetDebugName("PreDepthPass")
                    .SetDepthAttachment(DepthAttachment)
                    .SetDepthClearValue(0.0f)
                    .SetDepthLoadOp(ERenderLoadOp::Clear)
                    .SetDepthStoreOp(ERenderStoreOp::Store)
                    .SetRenderArea(GetRenderTarget()->GetExtent());
                
                GraphicsState.SetRenderPass(BeginInfo);
                
                GraphicsState.SetViewport(MakeViewportStateFromImage(GetRenderTarget()));
                
                FRenderState RenderState; RenderState
                    .SetDepthStencilState(FDepthStencilState().SetDepthFunc(EComparisonFunc::Greater))
                    .SetRasterState(FRasterState().EnableDepthClip());
                    
                FGraphicsPipelineDesc Desc;
                Desc.SetRenderState(RenderState);
                Desc.SetInputLayout(VertexLayoutInput);
                Desc.AddBindingLayout(BindingLayout);
                Desc.SetVertexShader(VertexShader);
                
                FRHIGraphicsPipelineRef Pipeline = GRenderContext->CreateGraphicsPipeline(Desc);
                
                GraphicsState.SetPipeline(Pipeline);
                GraphicsState.AddBindingSet(BindingSet);
                
                GraphicsState.SetIndirectParams(IndirectDrawBuffer);
                
                CmdList.SetGraphicsState(GraphicsState);
                
                SceneRenderStats.NumDrawCalls += Batch.NumDraws;
                uint64 Offset = Batch.Offset * sizeof(FDrawIndexedIndirectArguments);
                
                CmdList.DrawIndexedIndirect(Batch.NumDraws, Offset);
            }
        });

        RenderGraph.AddPass<RG_Raster>(FRGEvent("Base Pass"), nullptr, [&](ICommandList& CmdList)
        {
            LUMINA_PROFILE_SECTION_COLORED("Base Pass", tracy::Color::Red);
            
            if (RenderBatches.empty())
            {
                return;
            }
            
            FRenderPassBeginInfo BeginInfo; BeginInfo
            .SetDebugName("Base Pass")
            .AddColorAttachment(GBuffer.Position)
            .SetColorLoadOp(ERenderLoadOp::Clear)
            .SetColorStoreOp(ERenderStoreOp::Store)
            .SetColorClearColor(FColor::Black)
            
            .AddColorAttachment(GBuffer.Normals)
            .SetColorLoadOp(ERenderLoadOp::Clear)
            .SetColorStoreOp(ERenderStoreOp::Store)
            .SetColorClearColor(FColor::Black)
            
            .AddColorAttachment(GBuffer.Material)
            .SetColorLoadOp(ERenderLoadOp::Clear)
            .SetColorStoreOp(ERenderStoreOp::Store)
            .SetColorClearColor(FColor::Black)
            
            .AddColorAttachment(GBuffer.AlbedoSpec)
            .SetColorLoadOp(ERenderLoadOp::Clear)
            .SetColorStoreOp(ERenderStoreOp::Store)
            .SetColorClearColor(FColor::Black)
            
            .SetDepthAttachment(DepthAttachment)
            .SetDepthLoadOp(ERenderLoadOp::Load)
            .SetDepthStoreOp(ERenderStoreOp::Store)
                
            .SetRenderArea(GetRenderTarget()->GetExtent());


            FBlendState BlendState;
            FBlendState::RenderTarget PositionTarget;
            PositionTarget.SetFormat(EFormat::RGBA32_FLOAT);
            BlendState.SetRenderTarget(0, PositionTarget);
            
            FBlendState::RenderTarget NormalTarget;
            NormalTarget.SetFormat(EFormat::RGBA16_FLOAT);
            BlendState.SetRenderTarget(1, NormalTarget);
            
            FBlendState::RenderTarget MaterialTarget;
            MaterialTarget.SetFormat(EFormat::RGBA8_UNORM);
            BlendState.SetRenderTarget(2, MaterialTarget);
            
            FBlendState::RenderTarget AlbedoSpecTarget;
            AlbedoSpecTarget.SetFormat(EFormat::RGBA8_UNORM);
            BlendState.SetRenderTarget(3, AlbedoSpecTarget);
            
            FRasterState RasterState;
            RasterState.EnableDepthClip();

            FDepthStencilState DepthState; DepthState
                .SetDepthFunc(EComparisonFunc::Equal)
                .DisableDepthWrite();
            
            FRenderState RenderState;
            RenderState.SetBlendState(BlendState);
            RenderState.SetRasterState(RasterState);
            RenderState.SetDepthStencilState(DepthState);
            
            for (SIZE_T CurrentDraw = 0; CurrentDraw < RenderBatches.size(); ++CurrentDraw)
            {
                const FIndirectRenderBatch& Batch = RenderBatches[CurrentDraw];
                CMaterialInterface* Material = Batch.Material;

                FVertexBufferBinding VertexBufferBinding = FVertexBufferBinding()
                    .SetBuffer(Batch.StaticMesh->GetMeshResource().VertexBuffer);

                FIndexBufferBinding IndexBufferBinding = FIndexBufferBinding()
                    .SetBuffer(Batch.StaticMesh->GetMeshResource().IndexBuffer);

                FGraphicsState GraphicsState;
                GraphicsState.SetRenderPass(BeginInfo);
                GraphicsState.AddVertexBuffer(VertexBufferBinding);
                GraphicsState.SetIndexBuffer(IndexBufferBinding);
                GraphicsState.SetViewport(MakeViewportStateFromImage(GetRenderTarget()));

                
                FGraphicsPipelineDesc Desc;
                Desc.SetRenderState(RenderState);
                Desc.SetInputLayout(VertexLayoutInput);
                Desc.AddBindingLayout(BindingLayout);
                Desc.AddBindingLayout(Material->GetBindingLayout());
                Desc.SetVertexShader(Material->GetMaterial()->VertexShader);
                Desc.SetPixelShader(Material->GetMaterial()->PixelShader);
                
                FRHIGraphicsPipelineRef Pipeline = GRenderContext->CreateGraphicsPipeline(Desc);

                GraphicsState.SetPipeline(Pipeline);
                GraphicsState.SetIndirectParams(IndirectDrawBuffer);
                GraphicsState.AddBindingSet(BindingSet);
                GraphicsState.AddBindingSet(Material->GetBindingSet());
                CmdList.SetGraphicsState(GraphicsState);
                
                SceneRenderStats.NumDrawCalls += Batch.NumDraws;
                CmdList.DrawIndexedIndirect(Batch.NumDraws, Batch.Offset * sizeof(FDrawIndexedIndirectArguments));
            }
        });

        if (RenderSettings.bSSAO)
        {
            RenderGraph.AddPass<RG_Raster>(FRGEvent("SSAO Pass"), nullptr, [&] (ICommandList& CmdList)
            {
                LUMINA_PROFILE_SECTION_COLORED("SSAO Pass", tracy::Color::Pink);

                FRHIVertexShaderRef VertexShader = GRenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("FullscreenQuad.vert");
                FRHIPixelShaderRef PixelShader = GRenderContext->GetShaderLibrary()->GetShader<FRHIPixelShader>("SSAO.frag");
                if (!VertexShader || !PixelShader)
                {
                    return;
                }

                FRasterState RasterState;
                RasterState.SetCullNone();
    
                FBlendState BlendState;
                FBlendState::RenderTarget RenderTarget;
                RenderTarget.SetFormat(EFormat::R8_UNORM);
                BlendState.SetRenderTarget(0, RenderTarget);
    
                FDepthStencilState DepthState;
                DepthState.DisableDepthTest();
                DepthState.DisableDepthWrite();
            
                FRenderState RenderState;
                RenderState.SetRasterState(RasterState);
                RenderState.SetDepthStencilState(DepthState);
                RenderState.SetBlendState(BlendState);
            
                FGraphicsPipelineDesc Desc;
                Desc.SetRenderState(RenderState);
                Desc.AddBindingLayout(BindingLayout);
                Desc.AddBindingLayout(SSAOPassLayout);
                Desc.SetVertexShader(VertexShader);
                Desc.SetPixelShader(PixelShader);
    
                FRHIGraphicsPipelineRef Pipeline = GRenderContext->CreateGraphicsPipeline(Desc);

                FGraphicsState GraphicsState;
                GraphicsState.SetPipeline(Pipeline);
                GraphicsState.AddBindingSet(BindingSet);
                GraphicsState.AddBindingSet(SSAOPassSet);
            
                FRenderPassBeginInfo BeginInfo; BeginInfo
                .SetDebugName("SSAO Pass")
                .AddColorAttachment(SSAOImage)
                .SetColorLoadOp(ERenderLoadOp::Clear)
                .SetColorStoreOp(ERenderStoreOp::Store)
                .SetColorClearColor(FColor::Black)
                .SetRenderArea(GetRenderTarget()->GetExtent());

                GraphicsState.SetRenderPass(BeginInfo);
                GraphicsState.SetViewport(MakeViewportStateFromImage(GetRenderTarget()));

                CmdList.SetGraphicsState(GraphicsState);
            
                SceneRenderStats.NumDrawCalls++;
                SceneRenderStats.NumVertices += 3;
            
                CmdList.Draw(3, 1, 0, 0); 
            });


            RenderGraph.AddPass<RG_Raster>(FRGEvent("SSAO Blur Pass"), nullptr, [&](ICommandList& CmdList)
            {
                LUMINA_PROFILE_SECTION_COLORED("SSAO Blur Pass", tracy::Color::Yellow);

                FRHIVertexShaderRef VertexShader = GRenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("FullscreenQuad.vert");
                FRHIPixelShaderRef PixelShader = GRenderContext->GetShaderLibrary()->GetShader<FRHIPixelShader>("SSAOBlur.frag");
                if (!VertexShader || !PixelShader)
                {
                    return;
                }

                FRasterState RasterState;
                RasterState.SetCullNone();
    
                FBlendState BlendState;
                FBlendState::RenderTarget RenderTarget;
                RenderTarget.SetFormat(EFormat::R8_UNORM);
                BlendState.SetRenderTarget(0, RenderTarget);
    
                FDepthStencilState DepthState;
                DepthState.DisableDepthTest();
                DepthState.DisableDepthWrite();
            
                FRenderState RenderState;
                RenderState.SetRasterState(RasterState);
                RenderState.SetDepthStencilState(DepthState);
                RenderState.SetBlendState(BlendState);
            
                FGraphicsPipelineDesc Desc;
                Desc.SetRenderState(RenderState);
                Desc.AddBindingLayout(BindingLayout);
                Desc.AddBindingLayout(SSAOBlurPassLayout);
                Desc.SetVertexShader(VertexShader);
                Desc.SetPixelShader(PixelShader);
    
                FRHIGraphicsPipelineRef Pipeline = GRenderContext->CreateGraphicsPipeline(Desc);

                FGraphicsState GraphicsState;
                GraphicsState.SetPipeline(Pipeline);
                GraphicsState.AddBindingSet(BindingSet);
                GraphicsState.AddBindingSet(SSAOBlurPassSet);
            
            
                FRenderPassBeginInfo BeginInfo; BeginInfo
                .SetDebugName("SSAO Blue Pass")
                .AddColorAttachment(SSAOBlur)
                .SetColorLoadOp(ERenderLoadOp::Clear)
                .SetColorStoreOp(ERenderStoreOp::Store)
                .SetColorClearColor(FColor::Black)
                .SetRenderArea(GetRenderTarget()->GetExtent());
            
                GraphicsState.SetRenderPass(BeginInfo);
                GraphicsState.SetViewport(MakeViewportStateFromImage(GetRenderTarget()));

                CmdList.SetGraphicsState(GraphicsState);
            
                SceneRenderStats.NumDrawCalls++;
                SceneRenderStats.NumVertices += 3;
                CmdList.Draw(3, 1, 0, 0); 
            });
        }

        RenderGraph.AddPass<RG_Raster>(FRGEvent("Lighting Pass"), nullptr, [&](ICommandList& CmdList)
        {
            LUMINA_PROFILE_SECTION_COLORED("Lighting Pass", tracy::Color::Red2);
            
            FRHIVertexShaderRef VertexShader = GRenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("FullscreenQuad.vert");
            FRHIPixelShaderRef PixelShader = GRenderContext->GetShaderLibrary()->GetShader<FRHIPixelShader>("DeferredLighting.frag");
            if (!VertexShader || !PixelShader)
            {
                return;
            }
            
            FRasterState RasterState;
            RasterState.SetCullNone();
    
            FBlendState BlendState;
            FBlendState::RenderTarget RenderTarget;
            BlendState.SetRenderTarget(0, RenderTarget);
    
            FDepthStencilState DepthState;
            DepthState.DisableDepthTest();
            DepthState.DisableDepthWrite();
            
            FRenderState RenderState;
            RenderState.SetRasterState(RasterState);
            RenderState.SetDepthStencilState(DepthState);
            RenderState.SetBlendState(BlendState);
            
            FGraphicsPipelineDesc Desc;
            Desc.SetRenderState(RenderState);
            Desc.AddBindingLayout(BindingLayout);
            Desc.AddBindingLayout(LightingPassLayout);
            Desc.SetVertexShader(VertexShader);
            Desc.SetPixelShader(PixelShader);
    
            FRHIGraphicsPipelineRef Pipeline = GRenderContext->CreateGraphicsPipeline(Desc);

            FGraphicsState GraphicsState;
            GraphicsState.SetPipeline(Pipeline);
            GraphicsState.AddBindingSet(BindingSet);
            GraphicsState.AddBindingSet(LightingPassSet);
            
            FRenderPassBeginInfo BeginInfo; BeginInfo
            .SetDebugName("Lighting Pass")
            .AddColorAttachment(GetRenderTarget())
            .SetColorLoadOp(ERenderLoadOp::Clear)
            .SetColorStoreOp(ERenderStoreOp::Store)
            .SetColorClearColor(FColor::Black)
            
            .SetDepthAttachment(DepthAttachment)
            .SetDepthLoadOp(ERenderLoadOp::Load)
            .SetRenderArea(GetRenderTarget()->GetExtent());

            GraphicsState.SetRenderPass(BeginInfo);
            
            GraphicsState.SetViewport(MakeViewportStateFromImage(GetRenderTarget()));

            CmdList.SetGraphicsState(GraphicsState);

            SceneRenderStats.NumDrawCalls++;
            SceneRenderStats.NumVertices += 3;
            CmdList.Draw(3, 1, 0, 0); 
        });
        

        RenderGraph.AddPass<RG_Raster>(FRGEvent("Debug Draw Pass"), nullptr, [&](ICommandList& CmdList)
        {
            LUMINA_PROFILE_SECTION_COLORED("Debug Draw Pass", tracy::Color::Yellow3);
            
            FRHIVertexShaderRef VertexShader = GRenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("SimpleElement.vert");
            FRHIPixelShaderRef PixelShader = GRenderContext->GetShaderLibrary()->GetShader<FRHIPixelShader>("SimpleElement.frag");
            if (!VertexShader || !PixelShader || SimpleVertices.empty())
            {
                return;
            }
            
            FRasterState RasterState;
            RasterState.SetCullNone();
    
            FBlendState BlendState;
            FBlendState::RenderTarget RenderTarget;
            RenderTarget.DisableBlend();
            BlendState.SetRenderTarget(0, RenderTarget);
    
            FDepthStencilState DepthState;
            DepthState.EnableDepthTest();
            DepthState.SetDepthFunc(EComparisonFunc::GreaterOrEqual);
            DepthState.DisableDepthWrite();
            
            FRenderState RenderState;
            RenderState.SetRasterState(RasterState);
            RenderState.SetDepthStencilState(DepthState);
            RenderState.SetBlendState(BlendState);
            
            FGraphicsPipelineDesc Desc;
            Desc.SetPrimType(EPrimitiveType::LineList);
            Desc.SetInputLayout(SimpleVertexLayoutInput);
            Desc.SetRenderState(RenderState);
            Desc.AddBindingLayout(SimplePassLayout);
            Desc.SetVertexShader(VertexShader);
            Desc.SetPixelShader(PixelShader);
    
            FRHIGraphicsPipelineRef Pipeline = GRenderContext->CreateGraphicsPipeline(Desc);

            FGraphicsState GraphicsState;
            GraphicsState.SetPipeline(Pipeline);
            
            FRenderPassBeginInfo BeginInfo; BeginInfo
            .SetDebugName("Debug Draw Pass")
            .AddColorAttachment(GetRenderTarget())
            .SetColorLoadOp(ERenderLoadOp::Load)
            .SetColorStoreOp(ERenderStoreOp::Store)

            .SetDepthAttachment(DepthAttachment)
            .SetDepthLoadOp(ERenderLoadOp::Load)
            .SetDepthStoreOp(ERenderStoreOp::Store)
            .SetRenderArea(GetRenderTarget()->GetExtent());

            FVertexBufferBinding Binding;
            Binding.Buffer = SimpleVertexBuffer;
            GraphicsState.AddVertexBuffer(Binding);
            GraphicsState.SetRenderPass(BeginInfo);
            
            GraphicsState.SetViewport(MakeViewportStateFromImage(GetRenderTarget()));

            CmdList.SetGraphicsState(GraphicsState);

            glm::mat4 ViewProjMatrix = CameraComponent.GetViewProjectionMatrix();
            CmdList.SetPushConstants(&ViewProjMatrix, sizeof(glm::mat4));
            
            SceneRenderStats.NumDrawCalls++;
            SceneRenderStats.NumVertices += SimpleVertices.size();
            CmdList.Draw((uint32)SimpleVertices.size(), 1, 0, 0); 
        });
        
        
        RenderGraph.Execute();
        
        
        GraphicsCommandList->SetImageState(GetRenderTarget(), AllSubresources, EResourceStates::ShaderResource);
        GraphicsCommandList->CommitBarriers();

                
        GraphicsCommandList->SetImageState(DepthAttachment, AllSubresources, EResourceStates::ShaderResource);
        GraphicsCommandList->CommitBarriers();
    }

    void FSceneRenderer::OnSwapchainResized()
    {
        CreateImages();
    }
    
    FViewportState FSceneRenderer::MakeViewportStateFromImage(const FRHIImage* Image)
    {
        float SizeY = (float)Image->GetSizeY();
        float SizeX = (float)Image->GetSizeX();

        FViewportState ViewportState;
        ViewportState.Viewport = FViewport(SizeX, SizeY);
        ViewportState.Scissor = FRect(SizeX, SizeY);

        return ViewportState;
    }
    
    void FSceneRenderer::BuildPasses()
    {
        LUMINA_PROFILE_SCOPE();

        ICommandList* CommandList = GRenderContext->GetCommandList(ECommandQueue::Graphics);
        {
            RenderBatches.clear();
            RenderBatches.reserve(1000);
            
            StaticMeshRenders.clear();
            StaticMeshRenders.reserve(1000);
            
            IndirectDrawArguments.clear();
            IndirectDrawArguments.reserve(1000);
            
            InstanceData.clear();
            InstanceData.reserve(1000);
            LightData.NumLights = 0;

            TFixedVector<glm::mat4, 2000> Transforms;
            
            FMutex Mutex;
            auto Group = World->GetMutableEntityRegistry().group<SStaticMeshComponent>(entt::get<STransformComponent>);
            LUMINA_PROFILE_SECTION("Build Render Proxies");
            Group.each([&] (SStaticMeshComponent& MeshComponent, STransformComponent& TransformComponent)
            {
                CStaticMesh* Mesh = MeshComponent.StaticMesh;
                if (!IsValid(Mesh))
                {
                    return;
                }
                
                SIZE_T Surfaces = Mesh->GetMeshResource().GetNumSurfaces();
                for (SIZE_T j = 0; j < Surfaces; ++j)
                {
                    LUMINA_PROFILE_SECTION("Process Mesh Surface");

                    const FMeshResource& Resource = MeshComponent.StaticMesh->GetMeshResource();
                    if (!Resource.IsSurfaceIndexValid(j))
                    {
                        return;
                    }
            
                    const FGeometrySurface& Surface = Resource.GetSurface(j);
                    CMaterialInterface* Material = MeshComponent.GetMaterialForSlot(Surface.MaterialIndex);
                    if (!IsValid(Material) || !Material->IsReadyForRender())
                    {
                        continue;
                    }
                
                    FStaticMeshRender Proxy;
                    Proxy.StaticMesh = Mesh;
                    Proxy.Material = Material;
                    Proxy.FirstIndex = Surface.StartIndex;
                    Proxy.SurfaceIndexCount = Surface.IndexCount;
                    Proxy.TransformIdx = (uint32)Transforms.size();

                    Transforms.push_back(TransformComponent.GetMatrix());

                    {
                        // 64-bit key: [MaterialID:20 | MeshID:20 | FirstIndex:16 | IndexCount:8]
                        auto id32 = [](const void* p){ return uint64(reinterpret_cast<uintptr_t>(p)) & 0xFFFFF; }; // 20 bits
                        uint64 mat     = id32(Proxy.Material);
                        uint64 mesh    = id32(Proxy.StaticMesh);
                        uint64 first16 = uint64(Proxy.FirstIndex & 0xFFFF);
                        uint64 idx8    = uint64(eastl::min<uint32_t>(Surface.IndexCount, 0xFF));
                        Proxy.SortKey = (mat << 44) | (mesh << 24) | (first16 << 8) | idx8;
                    }

                    StaticMeshRenders.push_back(Proxy);
                }
            });
            
            
            {
                LUMINA_PROFILE_SECTION("Sort Render Proxies");
                eastl::sort(StaticMeshRenders.begin(), StaticMeshRenders.end());
            }
            
            
            {   
                LUMINA_PROFILE_SECTION("Build Indirect Draw Arguments");
                
                const FStaticMeshRender* PrevProxy = nullptr;

                for (const FStaticMeshRender& Render : StaticMeshRenders)
                {
                    if (!PrevProxy || (PrevProxy->SortKey != Render.SortKey))
                    {
                        FIndirectRenderBatch Batch;
                        Batch.NumDraws = 1;
                        Batch.StaticMesh = Render.StaticMesh;
                        Batch.Material = Render.Material;
                        Batch.Offset = IndirectDrawArguments.size();
                        RenderBatches.push_back(Batch);

                        FDrawIndexedIndirectArguments DrawArgument;
                        DrawArgument.BaseVertexLocation     = 0;
                        DrawArgument.StartIndexLocation     = Render.FirstIndex;
                        DrawArgument.IndexCount             = Render.SurfaceIndexCount;
                        DrawArgument.StartInstanceLocation  = (uint32)InstanceData.size();
                        DrawArgument.InstanceCount          = 1;

                        IndirectDrawArguments.push_back(DrawArgument);
                    }
                    else
                    {
                        IndirectDrawArguments.back().InstanceCount++;
                    }

                    InstanceData.emplace_back(Transforms[Render.TransformIdx]);
                    PrevProxy = &Render;
                }
            }
            
            if (!StaticMeshRenders.empty())
            {
                LUMINA_PROFILE_SECTION("Write Buffers");
                CommandList->WriteBuffer(ModelDataBuffer, InstanceData.data(), 0, InstanceData.size() * sizeof(FInstanceData));
                CommandList->WriteBuffer(IndirectDrawBuffer, IndirectDrawArguments.data(), 0, IndirectDrawArguments.size() * sizeof(FDrawIndexedIndirectArguments));
            }
        }

        //========================================================================================================================

        {
            auto Group = World->GetMutableEntityRegistry().group<SEnvironmentComponent>();
            if (!Group.empty())
            {
                SEnvironmentComponent& EnvironmentComponent = Group.get<SEnvironmentComponent>(Group[0]);
                RenderSettings.bSSAO = EnvironmentComponent.bSSAOEnabled;
                RenderSettings.SSAOSettings.Intensity = EnvironmentComponent.SSAOInfo.Intensity;
                RenderSettings.SSAOSettings.Power = EnvironmentComponent.SSAOInfo.Power;
                RenderSettings.SSAOSettings.Radius = EnvironmentComponent.SSAOInfo.Radius;
                CommandList->WriteBuffer(SSAOSettingsBuffer, &RenderSettings.SSAOSettings, 0, sizeof(FSSAOSettings));
            }
        }

        //========================================================================================================================
        
        {
            auto Group = World->GetMutableEntityRegistry().group<FLineBatcherComponent>();
            SimpleVertices.clear();
            Group.each([&](auto& LineBatcherComponent)
            {
                for (const FBatchedLine& Line : LineBatcherComponent.BatchedLines)
                {
                    SimpleVertices.emplace_back(glm::vec4(Line.Start, 1.0f), Line.Color);
                    SimpleVertices.emplace_back(glm::vec4(Line.End, 1.0f), Line.Color);
                }

                LineBatcherComponent.Flush();
            });

            CommandList->WriteBuffer(SimpleVertexBuffer, SimpleVertices.data(), 0, SimpleVertices.size() * sizeof(FSimpleElementVertex));
        }

        //========================================================================================================================
        
        {
            auto Group = World->GetMutableEntityRegistry().group<SPointLightComponent>(entt::get<STransformComponent>);
            for (uint64 i = 0; i < Group.size(); ++i)
            {
                entt::entity entity = Group[i];
                const SPointLightComponent& PointLightComponent = Group.get<SPointLightComponent>(entity);
                const STransformComponent& TransformComponent = Group.get<STransformComponent>(entity);

                FLight Light;
                Light.Type = LIGHT_TYPE_POINT;
                
                Light.Color = glm::vec4(PointLightComponent.LightColor, PointLightComponent.Intensity);
                Light.Radius = PointLightComponent.Attenuation;
                Light.Position = glm::vec4(TransformComponent.WorldTransform.Location, 1.0f);
                
                LightData.Lights[LightData.NumLights++] = Memory::Move(Light);

                //Scene->DrawDebugSphere(Transform.Location, 0.25f, Light.Color);
            }
        }

        //========================================================================================================================


        {
            auto Group = World->GetMutableEntityRegistry().group<>(entt::get<SCameraComponent>, entt::exclude<SEditorComponent>);
            for (uint64 i = 0; i < Group.size(); ++i)
            {
                entt::entity entity = Group[i];
                SCameraComponent& CameraComponent = Group.get<SCameraComponent>(entity);
                World->DrawArrow(CameraComponent.GetPosition(), CameraComponent.GetForwardVector(), 1.0f, FColor::Blue);
                World->DrawFrustum(CameraComponent.GetViewVolume().GetViewProjectionMatrix(), FColor::Green);
            }
        }

        //========================================================================================================================
        
        {
            auto Group = World->GetMutableEntityRegistry().group<SSpotLightComponent>(entt::get<STransformComponent>);
            for (uint64 i = 0; i < Group.size(); ++i)
            {
                entt::entity entity = Group[i];
                const SSpotLightComponent& SpotLightComponent = Group.get<SSpotLightComponent>(entity);
                const FTransform& Transform = Group.get<STransformComponent>(entity).WorldTransform;

                FLight SpotLight;
                SpotLight.Type = LIGHT_TYPE_SPOT;
                
                SpotLight.Position = glm::vec4(Transform.Location, 1.0f);

                glm::vec3 Forward = Transform.Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
                SpotLight.Direction = glm::vec4(glm::normalize(Forward), 0.0f);
                
                SpotLight.Color = glm::vec4(SpotLightComponent.LightColor, SpotLightComponent.Intensity);

                float InnerDegrees = SpotLightComponent.InnerConeAngle;
                float OuterDegrees = SpotLightComponent.OuterConeAngle;

                float InnerCos = glm::cos(glm::radians(InnerDegrees));
                float OuterCos = glm::cos(glm::radians(OuterDegrees));
                
                SpotLight.Angle = glm::vec2(InnerCos, OuterCos);

                SpotLight.Radius = SpotLightComponent.Attenuation;

                LightData.Lights[LightData.NumLights++] = SpotLight;

                //Scene->DrawDebugCone(SpotLight.Position, Forward, glm::radians(OuterDegrees), SpotLightComponent.Attenuation, glm::vec4(SpotLightComponent.LightColor, 1.0f));
                //Scene->DrawDebugCone(SpotLight.Position, Forward, glm::radians(InnerDegrees), SpotLightComponent.Attenuation, glm::vec4(SpotLightComponent.LightColor, 1.0f));

            }
        }
        

        {
            auto Group = World->GetMutableEntityRegistry().group<SDirectionalLightComponent>(entt::get<STransformComponent>);
            for (uint64 i = 0; i < Group.size(); ++i)
            {
                entt::entity entity = Group[i];

                const SDirectionalLightComponent& DirectionalLightComponent = Group.get<SDirectionalLightComponent>(entity);
                const FTransform& Transform = Group.get<STransformComponent>(entity).WorldTransform;

                FLight DirectionalLight;
                DirectionalLight.Type = LIGHT_TYPE_DIRECTIONAL;
                
                DirectionalLight.Color = glm::vec4(DirectionalLightComponent.Color, DirectionalLightComponent.Intensity);
                glm::vec3 Forward = Transform.Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
                DirectionalLight.Direction = glm::vec4(glm::normalize(Forward), 0.0f);
                
                LightData.Lights[LightData.NumLights++] = Memory::Move(DirectionalLight);
            }
        }

        CommandList->WriteBuffer(LightDataBuffer, &LightData, 0, sizeof(FSceneLightData));
    }

    void FSceneRenderer::BuildDrawCalls()
    {
        
    }

    void FSceneRenderer::InitResources()
    {
        {
            FVertexAttributeDesc VertexDesc[4];
            // Pos
            VertexDesc[0].SetElementStride(sizeof(FVertex));
            VertexDesc[0].SetOffset(offsetof(FVertex, Position));
            VertexDesc[0].Format = EFormat::RGBA32_FLOAT;

            // Color
            VertexDesc[1].SetElementStride(sizeof(FVertex));
            VertexDesc[1].SetOffset(offsetof(FVertex, Color));
            VertexDesc[1].Format = EFormat::RGBA32_FLOAT;

            // Normal
            VertexDesc[2].SetElementStride(sizeof(FVertex));
            VertexDesc[2].SetOffset(offsetof(FVertex, Normal));
            VertexDesc[2].Format = EFormat::RGBA32_FLOAT;

            // UV
            VertexDesc[3].SetElementStride(sizeof(FVertex));
            VertexDesc[3].SetOffset(offsetof(FVertex, UV));
            VertexDesc[3].Format = EFormat::RG32_FLOAT;

        
            VertexLayoutInput = GRenderContext->CreateInputLayout(VertexDesc, std::size(VertexDesc));
        }

        {
            FVertexAttributeDesc VertexDesc[2];
            // Pos
            VertexDesc[0].SetElementStride(sizeof(FSimpleElementVertex));
            VertexDesc[0].SetOffset(offsetof(FSimpleElementVertex, Position));
            VertexDesc[0].Format = EFormat::RGBA32_FLOAT;

            // Color
            VertexDesc[1].SetElementStride(sizeof(FSimpleElementVertex));
            VertexDesc[1].SetOffset(offsetof(FSimpleElementVertex, Color));
            VertexDesc[1].Format = EFormat::RGBA32_FLOAT;

            SimpleVertexLayoutInput = GRenderContext->CreateInputLayout(VertexDesc, std::size(VertexDesc));
        }
        
        {
            FBindingLayoutDesc BindingLayoutDesc;
            BindingLayoutDesc.StageFlags.SetMultipleFlags(ERHIShaderType::Vertex, ERHIShaderType::Fragment);
            BindingLayoutDesc.AddItem(FBindingLayoutItem::Buffer_UD(0));
            BindingLayoutDesc.AddItem(FBindingLayoutItem::Buffer_SRV(1));
            BindingLayoutDesc.AddItem(FBindingLayoutItem::Buffer_SRV(2));
            BindingLayoutDesc.AddItem(FBindingLayoutItem::PushConstants(0, 80));
            BindingLayout = GRenderContext->CreateBindingLayout(BindingLayoutDesc);

            FBindingSetDesc BindingSetDesc;
            BindingSetDesc.AddItem(FBindingSetItem::BufferCBV(0, SceneDataBuffer));
            BindingSetDesc.AddItem(FBindingSetItem::BufferSRV(1, ModelDataBuffer));
            BindingSetDesc.AddItem(FBindingSetItem::BufferSRV(2, LightDataBuffer));
        
            BindingSet = GRenderContext->CreateBindingSet(BindingSetDesc, BindingLayout);
        }

        {
            FBindingLayoutDesc LayoutDesc;
            LayoutDesc.AddItem(FBindingLayoutItem::Texture_SRV(0));
            LayoutDesc.AddItem(FBindingLayoutItem::Texture_SRV(1));
            LayoutDesc.AddItem(FBindingLayoutItem::Texture_SRV(2));
            LayoutDesc.AddItem(FBindingLayoutItem::Texture_SRV(3));
            LayoutDesc.AddItem(FBindingLayoutItem::Texture_SRV(4));
            LayoutDesc.StageFlags.SetFlag(ERHIShaderType::Fragment);
            
            LightingPassLayout = GRenderContext->CreateBindingLayout(LayoutDesc);

            FBindingSetDesc SetDesc;
            SetDesc.AddItem(FBindingSetItem::TextureSRV(0, GBuffer.Position));
            SetDesc.AddItem(FBindingSetItem::TextureSRV(1, GBuffer.Normals));
            SetDesc.AddItem(FBindingSetItem::TextureSRV(2, GBuffer.Material));
            SetDesc.AddItem(FBindingSetItem::TextureSRV(3, GBuffer.AlbedoSpec));
            SetDesc.AddItem(FBindingSetItem::TextureSRV(4, SSAOBlur));

            LightingPassSet = GRenderContext->CreateBindingSet(SetDesc, LightingPassLayout);
        }

        {
            FBindingLayoutDesc LayoutDesc;
            LayoutDesc.StageFlags.SetFlag(ERHIShaderType::Vertex);
            LayoutDesc.AddItem(FBindingLayoutItem::PushConstants(0, 80));
            SimplePassLayout = GRenderContext->CreateBindingLayout(LayoutDesc);
        }

        {
            FBindingLayoutDesc LayoutDesc;
            LayoutDesc.AddItem(FBindingLayoutItem::Texture_SRV(0));
            LayoutDesc.AddItem(FBindingLayoutItem::Texture_SRV(1));
            LayoutDesc.AddItem(FBindingLayoutItem::Texture_SRV(2));
            LayoutDesc.AddItem(FBindingLayoutItem::Buffer_CBV(3));
            LayoutDesc.AddItem(FBindingLayoutItem::Buffer_UD(4));
            LayoutDesc.StageFlags.SetFlag(ERHIShaderType::Fragment);
            
            SSAOPassLayout = GRenderContext->CreateBindingLayout(LayoutDesc);

            FBindingSetDesc SetDesc;
            SetDesc.AddItem(FBindingSetItem::TextureSRV(0, GBuffer.Position));
            SetDesc.AddItem(FBindingSetItem::TextureSRV(1, GBuffer.Normals));
            SetDesc.AddItem(FBindingSetItem::TextureSRV(2, NoiseImage, TStaticRHISampler<true, AM_Repeat, AM_Repeat, AM_Repeat>::GetRHI()));
            SetDesc.AddItem(FBindingSetItem::BufferCBV(3, SSAOKernalBuffer));
            SetDesc.AddItem(FBindingSetItem::BufferCBV(4, SSAOSettingsBuffer));

            SSAOPassSet = GRenderContext->CreateBindingSet(SetDesc, SSAOPassLayout);
        }

        {
            FBindingLayoutDesc LayoutDesc;
            LayoutDesc.AddItem(FBindingLayoutItem::Texture_SRV(0));
            LayoutDesc.StageFlags.SetFlag(ERHIShaderType::Fragment);
            
            SSAOBlurPassLayout = GRenderContext->CreateBindingLayout(LayoutDesc);

            FBindingSetDesc SetDesc;
            SetDesc.AddItem(FBindingSetItem::TextureSRV(0, SSAOImage));

            SSAOBlurPassSet = GRenderContext->CreateBindingSet(SetDesc, SSAOBlurPassLayout);
        }
        
        {
            FBindingLayoutDesc SkyboxLayoutDesc;
            SkyboxLayoutDesc.AddItem(FBindingLayoutItem::Texture_SRV(0));
            SkyboxLayoutDesc.StageFlags.SetFlag(ERHIShaderType::Fragment);
            SkyboxBindingLayout = GRenderContext->CreateBindingLayout(SkyboxLayoutDesc);

            FBindingSetDesc SkyboxSetDesc;
            SkyboxSetDesc.AddItem(FBindingSetItem::TextureSRV(0, CubeMap));
            SkyboxBindingSet = GRenderContext->CreateBindingSet(SkyboxSetDesc, SkyboxBindingLayout);
        }
        
    }

    static float lerp(float a, float b, float f)
    {
        return a + f * (b - a);
    }

    void FSceneRenderer::InitBuffers()
    {
        {
            FRHIBufferDesc BufferDesc;
            BufferDesc.Size = sizeof(FSceneGlobalData);
            BufferDesc.Stride = sizeof(FSceneGlobalData);
            BufferDesc.Usage.SetMultipleFlags(BUF_UniformBuffer, BUF_Dynamic);
            BufferDesc.MaxVersions = 3;
            BufferDesc.DebugName = "Scene Global Data";
            SceneDataBuffer = GRenderContext->CreateBuffer(BufferDesc);
            GRenderContext->SetObjectName(SceneDataBuffer, BufferDesc.DebugName.c_str(), EAPIResourceType::Buffer);
        }

        {
            FRHIBufferDesc ModelBufferDesc;
            ModelBufferDesc.Size = sizeof(glm::mat4) * 100000;
            ModelBufferDesc.Stride = sizeof(glm::mat4);
            ModelBufferDesc.Usage.SetMultipleFlags(BUF_StorageBuffer);
            ModelBufferDesc.bKeepInitialState = true;
            ModelBufferDesc.InitialState = EResourceStates::ShaderResource;
            ModelBufferDesc.DebugName = "Model Buffer";
            ModelDataBuffer = GRenderContext->CreateBuffer(ModelBufferDesc);
            GRenderContext->SetObjectName(ModelDataBuffer, ModelBufferDesc.DebugName.c_str(), EAPIResourceType::Buffer);
        }

        {
            FRHIBufferDesc LightBufferDesc;
            LightBufferDesc.Size = sizeof(FSceneLightData);
            LightBufferDesc.Stride = sizeof(FSceneLightData);
            LightBufferDesc.Usage.SetMultipleFlags(BUF_StorageBuffer);
            LightBufferDesc.bKeepInitialState = true;
            LightBufferDesc.InitialState = EResourceStates::ShaderResource;
            LightBufferDesc.DebugName = "Light Data Buffer";
            LightDataBuffer = GRenderContext->CreateBuffer(LightBufferDesc);
            GRenderContext->SetObjectName(LightDataBuffer, LightBufferDesc.DebugName.c_str(), EAPIResourceType::Buffer);
        }

        {
            FRHIBufferDesc BufferDesc;
            BufferDesc.Size = sizeof(FSSAOSettings);
            BufferDesc.Stride = sizeof(FSSAOSettings);
            BufferDesc.Usage.SetMultipleFlags(BUF_UniformBuffer, BUF_Dynamic);
            BufferDesc.MaxVersions = 3;
            BufferDesc.DebugName = "SSAO Settings";
            SSAOSettingsBuffer = GRenderContext->CreateBuffer(BufferDesc);
            GRenderContext->SetObjectName(SSAOSettingsBuffer, BufferDesc.DebugName.c_str(), EAPIResourceType::Buffer);
        }

        {
            
            // SSAO
            std::default_random_engine rndEngine;
            std::uniform_real_distribution rndDist(0.0f, 1.0f);

            // Sample kernel
            TVector<glm::vec4> SSAOKernel(32);
            for (uint32_t i = 0; i < 32; ++i)
            {
                glm::vec3 sample(rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine));
                sample = glm::normalize(sample);
                sample *= rndDist(rndEngine);
                float scale = float(i) / float(32);
                scale = lerp(0.1f, 1.0f, scale * scale);
                SSAOKernel[i] = glm::vec4(sample * scale, 0.0f);
            }

            FRHICommandListRef CommandList = GRenderContext->CreateCommandList(FCommandListInfo::Transfer());
            CommandList->Open();
        
            FRHIBufferDesc SSAOBufferDesc;
            SSAOBufferDesc.Size = SSAOKernel.size() * sizeof(glm::vec4);
            SSAOBufferDesc.Stride = sizeof(glm::vec4);
            SSAOBufferDesc.Usage.SetMultipleFlags(BUF_UniformBuffer);
            SSAOBufferDesc.bKeepInitialState = true;
            SSAOBufferDesc.InitialState = EResourceStates::ShaderResource;
            SSAOBufferDesc.DebugName = "SSAO Kernal Buffer";
            SSAOKernalBuffer = GRenderContext->CreateBuffer(SSAOBufferDesc);
            GRenderContext->SetObjectName(SSAOKernalBuffer, SSAOBufferDesc.DebugName.c_str(), EAPIResourceType::Buffer);

            CommandList->WriteBuffer(SSAOKernalBuffer, SSAOKernel.data(), 0, SSAOBufferDesc.Size);
            
            CommandList->Close();
            GRenderContext->ExecuteCommandList(CommandList, ECommandQueue::Transfer);

            CommandList = GRenderContext->CreateCommandList(FCommandListInfo::Graphics());
            CommandList->Open();
            
            FRHIImageDesc SSAONoiseDesc = {};
            SSAONoiseDesc.Extent = {4, 4};
            SSAONoiseDesc.Format = EFormat::RGBA32_FLOAT;
            SSAONoiseDesc.Dimension = EImageDimension::Texture2D;
            SSAONoiseDesc.bKeepInitialState = true;
            SSAONoiseDesc.InitialState = EResourceStates::ShaderResource;
            SSAONoiseDesc.Flags.SetMultipleFlags(EImageCreateFlags::ShaderResource);
            SSAONoiseDesc.DebugName = "SSAO Noise";
        
            NoiseImage = GRenderContext->CreateImage(SSAONoiseDesc);
            GRenderContext->SetObjectName(NoiseImage, "SSAO Noise", EAPIResourceType::Image);
        
            // Random noise
            TVector<glm::vec4> NoiseValues(32);
            for (SIZE_T i = 0; i < NoiseValues.size(); i++)
            {
                NoiseValues[i] = glm::vec4(rndDist(rndEngine) * 2.0f - 1.0f, rndDist(rndEngine) * 2.0f - 1.0f, 0.0f, 0.0f);
            }
            
            CommandList->WriteImage(NoiseImage, 0, 0, NoiseValues.data(), 4 * 16, 0);

            CommandList->Close();
            GRenderContext->ExecuteCommandList(CommandList, ECommandQueue::Graphics);
        }
        

        {
            FRHIBufferDesc VertexBufferDesc;
            VertexBufferDesc.Size = sizeof(FSimpleElementVertex) * (1024 * 1024 * 10);
            VertexBufferDesc.Stride = sizeof(FSimpleElementVertex);
            VertexBufferDesc.Usage.SetMultipleFlags(BUF_VertexBuffer);
            VertexBufferDesc.InitialState = EResourceStates::VertexBuffer;
            VertexBufferDesc.bKeepInitialState = true;
            VertexBufferDesc.DebugName = "Simple Vertex Buffer";
            SimpleVertexBuffer = GRenderContext->CreateBuffer(VertexBufferDesc);
            GRenderContext->SetObjectName(SimpleVertexBuffer, VertexBufferDesc.DebugName.c_str(), EAPIResourceType::Buffer);
        }
        

        {
            FRHIBufferDesc IndirectBufferDesc;
            IndirectBufferDesc.Size = sizeof(FDrawIndexedIndirectArguments) * (1024 * 1024 * 10);
            IndirectBufferDesc.Stride = sizeof(uint32);
            IndirectBufferDesc.Usage.SetMultipleFlags(BUF_Indirect);
            IndirectBufferDesc.InitialState = EResourceStates::IndexBuffer;
            IndirectBufferDesc.bKeepInitialState = true;
            IndirectBufferDesc.DebugName = "Indirect Draw Buffer";
            IndirectDrawBuffer = GRenderContext->CreateBuffer(IndirectBufferDesc);
            GRenderContext->SetObjectName(IndirectDrawBuffer, IndirectBufferDesc.DebugName.c_str(), EAPIResourceType::Buffer);
        }
    }
    
    void FSceneRenderer::CreateImages()
    {
        FIntVector2D Extent = Windowing::GetPrimaryWindowHandle()->GetExtent();

        {
            FRHIImageDesc GBufferPosition;
            GBufferPosition.Extent = Extent;
            GBufferPosition.Flags.SetMultipleFlags(EImageCreateFlags::ColorAttachment, EImageCreateFlags::ShaderResource);
            GBufferPosition.Format = EFormat::RGBA32_FLOAT;
            GBufferPosition.Dimension = EImageDimension::Texture2D;
            GBufferPosition.DebugName = "GBuffer - Position";
        
            GBuffer.Position = GRenderContext->CreateImage(GBufferPosition);
            GRenderContext->SetObjectName(GBuffer.Position, "GBuffer - Position", EAPIResourceType::Image);
        }

        {
            FRHIImageDesc NormalDesc = {};
            NormalDesc.Extent = Extent;
            NormalDesc.Format = EFormat::RGBA16_FLOAT;
            NormalDesc.Dimension = EImageDimension::Texture2D;
            NormalDesc.Flags.SetMultipleFlags(EImageCreateFlags::ColorAttachment, EImageCreateFlags::ShaderResource);
            NormalDesc.DebugName = "GBuffer - Normals";
        
            GBuffer.Normals = GRenderContext->CreateImage(NormalDesc);
            GRenderContext->SetObjectName(GBuffer.Normals, "GBuffer - Normals", EAPIResourceType::Image);
        }

        {
            FRHIImageDesc MaterialDesc = {};
            MaterialDesc.Extent = Extent;
            MaterialDesc.Format = EFormat::RGBA8_UNORM;
            MaterialDesc.Dimension = EImageDimension::Texture2D;
            MaterialDesc.Flags.SetMultipleFlags(EImageCreateFlags::ColorAttachment, EImageCreateFlags::ShaderResource);
            MaterialDesc.DebugName = "GBuffer - Material";
        
            GBuffer.Material = GRenderContext->CreateImage(MaterialDesc);
            GRenderContext->SetObjectName(GBuffer.Material, "GBuffer - Material", EAPIResourceType::Image);
        }

        {
            FRHIImageDesc AlbedoDesc = {};
            AlbedoDesc.Extent = Extent;
            AlbedoDesc.Format = EFormat::RGBA8_UNORM;
            AlbedoDesc.Dimension = EImageDimension::Texture2D;
            AlbedoDesc.Flags.SetMultipleFlags(EImageCreateFlags::ColorAttachment, EImageCreateFlags::ShaderResource);
            AlbedoDesc.DebugName = "GBuffer - Albedo";
        
            GBuffer.AlbedoSpec = GRenderContext->CreateImage(AlbedoDesc);
            GRenderContext->SetObjectName(GBuffer.AlbedoSpec, "GBuffer - Albedo", EAPIResourceType::Image);
        }
        

        {
            FRHIImageDesc DepthImageDesc;
            DepthImageDesc.Extent = Extent;
            DepthImageDesc.Flags.SetMultipleFlags(EImageCreateFlags::DepthAttachment, EImageCreateFlags::ShaderResource);
            DepthImageDesc.Format = EFormat::D32;
            DepthImageDesc.InitialState = EResourceStates::DepthWrite;
            DepthImageDesc.bKeepInitialState = true;
            DepthImageDesc.Dimension = EImageDimension::Texture2D;
            DepthImageDesc.DebugName = "Depth Attachment";

            DepthAttachment = GRenderContext->CreateImage(DepthImageDesc);
            GRenderContext->SetObjectName(DepthAttachment, "Depth Attachment", EAPIResourceType::Image);
        }

        {
            FRHIImageDesc SSAODesc = {};
            SSAODesc.Extent = Extent;
            SSAODesc.Format = EFormat::R8_UNORM;
            SSAODesc.Dimension = EImageDimension::Texture2D;
            SSAODesc.Flags.SetMultipleFlags(EImageCreateFlags::ColorAttachment, EImageCreateFlags::ShaderResource);
            SSAODesc.DebugName = "SSAO";
        
            SSAOImage = GRenderContext->CreateImage(SSAODesc);
            GRenderContext->SetObjectName(SSAOImage, "SSAO", EAPIResourceType::Image);
        }

        {
            FRHIImageDesc SSAODesc = {};
            SSAODesc.Extent = Extent;
            SSAODesc.Format = EFormat::R8_UNORM;
            SSAODesc.Dimension = EImageDimension::Texture2D;
            SSAODesc.Flags.SetMultipleFlags(EImageCreateFlags::ColorAttachment, EImageCreateFlags::ShaderResource);
            SSAODesc.DebugName = "SSAO Blur";
        
            SSAOBlur = GRenderContext->CreateImage(SSAODesc);
            GRenderContext->SetObjectName(SSAOBlur, "SSAO Blur", EAPIResourceType::Image);
        }
        
        
        //==================================================================================================

        {
            FRHIImageDesc SkyCubeMapDesc;
            SkyCubeMapDesc.Extent = {2048, 2048};
            SkyCubeMapDesc.Flags.SetFlag(EImageCreateFlags::CubeCompatible);
            SkyCubeMapDesc.Flags.SetFlag(EImageCreateFlags::ShaderResource);
            SkyCubeMapDesc.Format = EFormat::RGBA8_UNORM;
            SkyCubeMapDesc.Dimension = EImageDimension::Texture2D;
            SkyCubeMapDesc.ArraySize = 6;
            SkyCubeMapDesc.DebugName = "Skybox CubeMap";

            CubeMap = GRenderContext->CreateImage(SkyCubeMapDesc);
            GRenderContext->SetObjectName(CubeMap, "CubeMap", EAPIResourceType::Image);

            static const char* CubeFaceFiles[6] = {
                "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg"
            };

            FRHICommandListRef CommandList = GRenderContext->CreateCommandList(FCommandListInfo::Transfer());
            CommandList->Open();

            CommandList->BeginTrackingImageState(CubeMap, AllSubresources, EResourceStates::Common);
        
            for (int i = 0; i < 6; ++i)
            {
                FString ResourceDirectory = Paths::GetEngineResourceDirectory();
                ResourceDirectory += FString("/Textures/CubeMaps/Mountains/") + CubeFaceFiles[i];
            
                TVector<uint8> Pixels;
                FIntVector2D ImageExtent = Import::Textures::ImportTexture(Pixels, ResourceDirectory, false);
            
                const uint32 width = ImageExtent.X;
                const uint32 height = ImageExtent.Y;
                const SIZE_T rowPitch = width * 4;  // 4 bytes per pixel (RGBA8)
                const SIZE_T depthPitch = rowPitch * height;
            
                CommandList->WriteImage(CubeMap, i, 0, Pixels.data(), rowPitch, depthPitch);
            }

            CommandList->SetPermanentImageState(CubeMap, EResourceStates::ShaderResource);
            CommandList->CommitBarriers();
        
            CommandList->Close();
            GRenderContext->ExecuteCommandList(CommandList, ECommandQueue::Transfer);
        }

        //==================================================================================================

        {
            FRHIImageDesc DepthMapDesc;
            DepthMapDesc.Extent = Extent;
            DepthMapDesc.Flags.SetMultipleFlags(EImageCreateFlags::DepthAttachment, EImageCreateFlags::ShaderResource);
            DepthMapDesc.Format = EFormat::D32;
            DepthMapDesc.InitialState = EResourceStates::DepthWrite;
            DepthMapDesc.bKeepInitialState = true;
            DepthMapDesc.Dimension = EImageDimension::Texture2D;
            DepthMapDesc.DebugName = "Depth Map";

            DepthMap = GRenderContext->CreateImage(DepthMapDesc);
            GRenderContext->SetObjectName(DepthMap, "Depth Map", EAPIResourceType::Image);
        }

        //==================================================================================================
        
        {
            FRHIImageDesc CubeMapDesc;
            CubeMapDesc.Extent = {1024, 1024};
            CubeMapDesc.Flags.SetFlag(EImageCreateFlags::CubeCompatible);
            CubeMapDesc.Flags.SetFlag(EImageCreateFlags::ShaderResource);
            CubeMapDesc.Format = EFormat::RGBA8_UNORM;
            CubeMapDesc.bKeepInitialState = true;
            CubeMapDesc.InitialState = EResourceStates::ShaderResource;
            CubeMapDesc.Dimension = EImageDimension::Texture2D;
            CubeMapDesc.ArraySize = 6;
            CubeMapDesc.DebugName = "Shadow Cubemap";

            ShadowCubeMap = GRenderContext->CreateImage(CubeMapDesc);
            GRenderContext->SetObjectName(ShadowCubeMap, CubeMapDesc.DebugName.c_str(), EAPIResourceType::Image);
        }
        
    }
    
}
