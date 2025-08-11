#include "SceneRenderer.h"

#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Core/Windows/Window.h"
#include "Entity/Components/CameraComponent.h"
#include "Entity/Entity.h"
#include "Renderer/RHIIncl.h"
#include "Scene.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <execution>

#include "Assets/AssetTypes/Material/Material.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "glm/gtx/quaternion.hpp"
#include "Core/Engine/Engine.h"
#include "Core/Profiler/Profile.h"
#include "Entity/Components/LightComponent.h"
#include "Entity/Components/StaticMeshComponent.h"
#include "Paths/Paths.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"
#include "Renderer/ShaderCompiler.h"
#include "Renderer/RenderGraph/RenderGraph.h"
#include "Renderer/RenderGraph/RenderGraphDescriptor.h"
#include "Subsystems/FCameraManager.h"
#include "TaskSystem/TaskSystem.h"
#include "Tools/Import/ImportHelpers.h"

namespace Lumina
{
    
    FSceneRenderer::FSceneRenderer(FScene* InScene)
        : Scene(InScene)
        , SceneRenderStats()
        , SceneGlobalData()
    {
        SceneViewport = GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext()->CreateViewport(Windowing::GetPrimaryWindowHandle()->GetExtent());
    }

    FSceneRenderer::~FSceneRenderer()
    {
    }

    void FSceneRenderer::Initialize()
    {
        LUMINA_PROFILE_SCOPE();
        LOG_TRACE("Initializing Scene Renderer");
        
        RenderContext = GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext();
        
        // Wait for shader tasks.
        while (RenderContext->GetShaderCompiler()->HasPendingRequests()) {}
        
        InitBuffers();
        CreateImages();
        InitResources();
    }

    void FSceneRenderer::Deinitialize()
    {
        RenderContext->WaitIdle();
        
        LOG_TRACE("Shutting down scene renderer");
    }
    
    void FSceneRenderer::Render(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();
        SceneRenderStats = {};

        ICommandList* GraphicsCommandList = RenderContext->GetCommandList(Q_Graphics);
        
        FCameraManager* CameraManager = Scene->GetSceneSubsystem<FCameraManager>();
        SCameraComponent& CameraComponent = CameraManager->GetActiveCameraEntity().GetComponent<SCameraComponent>();

        SceneGlobalData.CameraData.Location =   glm::vec4(CameraComponent.GetPosition(), 1.0f);
        SceneGlobalData.CameraData.View =       CameraComponent.GetViewMatrix();
        SceneGlobalData.CameraData.InverseView = CameraComponent.GetViewVolume().GetInverseViewMatrix();
        SceneGlobalData.CameraData.Projection = CameraComponent.GetProjectionMatrix();
        SceneGlobalData.CameraData.InverseProjection = CameraComponent.GetViewVolume().GetInverseProjectionMatrix();
        SceneGlobalData.Time =                  (float)Scene->GetTimeSinceSceneCreation();
        SceneGlobalData.DeltaTime =             (float)Scene->GetSceneDeltaTime();
        SceneGlobalData.FarPlane =              0.01f;
        SceneGlobalData.NearPlane =             1000.0f;
        
        SceneViewport->SetViewVolume(CameraComponent.GetViewVolume());

        BuildPasses();
        

        FRenderGraph RenderGraph;
        
        RenderGraph.AddPass<RG_Transfer>(FRGEvent("Write Scene Buffer"), nullptr, [&](ICommandList& CmdList)
        {
            GraphicsCommandList->WriteBuffer(SceneDataBuffer, &SceneGlobalData, 0, sizeof(FSceneGlobalData));
        });
        
        RenderGraph.AddPass<RG_Raster>(FRGEvent("Pre-Depth Pass"), nullptr, [&] (ICommandList& CmdList)
        {
            LUMINA_PROFILE_SECTION_COLORED("Pre-Depth Pass", tracy::Color::Orange);
            
            FRHIVertexShaderRef VertexShader = RenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("DepthPrePass.vert");
            if (!VertexShader)
            {
                return;
            }
            
            for (SIZE_T CurrentDraw = 0; CurrentDraw < RenderBatches.size(); ++CurrentDraw)
            {
                const FIndirectRenderBatch& Batch = RenderBatches[CurrentDraw];

                FGraphicsState GraphicsState;
            
                FVertexBufferBinding VertexBufferBinding;
                VertexBufferBinding.SetBuffer(VertexBuffer);
                GraphicsState.AddVertexBuffer(VertexBufferBinding);
                
                FIndexBufferBinding IndexBufferBinding;
                IndexBufferBinding.SetBuffer(IndexBuffer);
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
                
                FRHIGraphicsPipelineRef Pipeline = RenderContext->CreateGraphicsPipeline(Desc);
                
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
            
            SceneRenderStats.NumVertices = Vertices.size();
            SceneRenderStats.NumIndices = Indices.size();
            
            FVertexBufferBinding VertexBufferBinding;
            VertexBufferBinding.SetBuffer(VertexBuffer);

            FIndexBufferBinding IndexBufferBinding;
            IndexBufferBinding.SetBuffer(IndexBuffer);
            
            for (SIZE_T CurrentDraw = 0; CurrentDraw < RenderBatches.size(); ++CurrentDraw)
            {
                const FIndirectRenderBatch& Batch = RenderBatches[CurrentDraw];
                CMaterialInterface* Material = Batch.Material;

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
                
                FRHIGraphicsPipelineRef Pipeline = RenderContext->CreateGraphicsPipeline(Desc);

                GraphicsState.SetPipeline(Pipeline);
                GraphicsState.SetIndirectParams(IndirectDrawBuffer);
                GraphicsState.AddBindingSet(BindingSet);
                GraphicsState.AddBindingSet(Material->GetBindingSet());
                CmdList.SetGraphicsState(GraphicsState);
                
                SceneRenderStats.NumDrawCalls += Batch.NumDraws;
                CmdList.DrawIndexedIndirect(Batch.NumDraws, Batch.Offset * sizeof(FDrawIndexedIndirectArguments));
            }
        });

        RenderGraph.AddPass<RG_Raster>(FRGEvent("SSAO Pass"), nullptr, [&] (ICommandList& CmdList)
        {
            LUMINA_PROFILE_SECTION_COLORED("SSAO Pass", tracy::Color::Pink);

            FRHIVertexShaderRef VertexShader = RenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("FullscreenQuad.vert");
            FRHIPixelShaderRef PixelShader = RenderContext->GetShaderLibrary()->GetShader<FRHIPixelShader>("SSAO.frag");
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
    
            FRHIGraphicsPipelineRef Pipeline = RenderContext->CreateGraphicsPipeline(Desc);

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

            FRHIVertexShaderRef VertexShader = RenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("FullscreenQuad.vert");
            FRHIPixelShaderRef PixelShader = RenderContext->GetShaderLibrary()->GetShader<FRHIPixelShader>("SSAOBlur.frag");
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
    
            FRHIGraphicsPipelineRef Pipeline = RenderContext->CreateGraphicsPipeline(Desc);

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


        RenderGraph.AddPass<RG_Raster>(FRGEvent("Lighting Pass"), nullptr, [&](ICommandList& CmdList)
        {
            LUMINA_PROFILE_SECTION_COLORED("Lighting Pass", tracy::Color::Red2);
            
            FRHIVertexShaderRef VertexShader = RenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("FullscreenQuad.vert");
            FRHIPixelShaderRef PixelShader = RenderContext->GetShaderLibrary()->GetShader<FRHIPixelShader>("DeferredLighting.frag");
            if (!VertexShader || !PixelShader)
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
    
            FRHIGraphicsPipelineRef Pipeline = RenderContext->CreateGraphicsPipeline(Desc);

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

    bool FSceneRenderer::ResizeBufferIfNeeded(FRHIBufferRef& Buffer, SIZE_T DesiredSize) const
    {
        if (DesiredSize != 0)
        {
            if (Buffer->GetSize() < DesiredSize)
            {
                FRHIBufferDesc BufferDesc = Buffer->GetDescription();
                BufferDesc.Size = DesiredSize;
                Buffer = RenderContext->CreateBuffer(BufferDesc);
                RenderContext->SetObjectName(VertexBuffer, BufferDesc.DebugName.c_str(), EAPIResourceType::Buffer);
                return true;
            }
        }

        return false;
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

    void FSceneRenderer::OnStaticMeshComponentCreated()
    {
    }

    void FSceneRenderer::OnStaticMeshComponentDestroyed()
    {
    }

    void FSceneRenderer::BuildPasses()
    {
        ICommandList* CommandList = RenderContext->GetCommandList(ECommandQueue::Graphics);
        {
            auto Group = Scene->GetMutableEntityRegistry().group<SStaticMeshComponent>(entt::get<STransformComponent>);
            SIZE_T WorkSize = Group.size();
            
            RenderBatches.clear();
            StaticMeshRenders.clear();
            IndirectDrawArguments.clear();
            ModelData.ModelMatrices.clear();
            LightData.NumLights = 0;
            
            {
                LUMINA_PROFILE_SECTION("Process Mesh Components");

                for (uint32 i = 0; i < WorkSize; ++i)
                {
                    entt::entity Entity = Group[i];
                    STransformComponent& TransformComponent = Group.get<STransformComponent>(Entity);
                    SStaticMeshComponent& MeshComponent = Group.get<SStaticMeshComponent>(Entity);

                    CStaticMesh* Mesh = MeshComponent.StaticMesh;
                    
                    if (!IsValid(Mesh))
                        continue;

                    if (RegisteredMeshes.find(Mesh) == RegisteredMeshes.end())
                    {
                        RegisteredMeshes.emplace(Mesh);

                        const auto& MeshVertices = Mesh->GetMeshResource().Vertices;
                        const auto& MeshIndices  = Mesh->GetMeshResource().Indices;

                        {
                            LUMINA_PROFILE_SECTION("Write Vertex Buffer");

                            SIZE_T WriteSize = MeshVertices.size() * sizeof(FVertex);
                            MeshVertexOffset.emplace(Mesh, NextVertexBufferWritePos);

                            Vertices.insert(Vertices.end(), MeshVertices.begin(), MeshVertices.end());
                            SIZE_T RequiredSize = (Vertices.size() * sizeof(FVertex)) * 1.25;

                            if (ResizeBufferIfNeeded(VertexBuffer, RequiredSize))
                            {
                                CommandList->WriteBuffer(VertexBuffer, Vertices.data(), 0, RequiredSize);
                            }
                            else
                            {
                                CommandList->WriteBuffer(VertexBuffer, MeshVertices.data(), NextVertexBufferWritePos, WriteSize);
                            }

                            NextVertexBufferWritePos += WriteSize;
                        }

                        {
                            LUMINA_PROFILE_SECTION("Write Index Buffer");

                            SIZE_T WriteSize = MeshIndices.size() * sizeof(uint32);
                            MeshIndexOffset.emplace(Mesh, NextIndexBufferWritePos);

                            Indices.insert(Indices.end(), MeshIndices.begin(), MeshIndices.end());
                            SIZE_T RequiredSize = (Indices.size() * sizeof(uint32)) * 1.25;

                            if (ResizeBufferIfNeeded(IndexBuffer, RequiredSize))
                            {
                                CommandList->WriteBuffer(IndexBuffer, Indices.data(), 0, RequiredSize);
                            }
                            else
                            {
                                CommandList->WriteBuffer(IndexBuffer, MeshIndices.data(), NextIndexBufferWritePos, WriteSize);
                            }

                            NextIndexBufferWritePos += WriteSize;
                        }
                    }

                    {
                        LUMINA_PROFILE_SECTION("Build Render Proxies");

                        SIZE_T Surfaces = Mesh->GetMeshResource().GetNumSurfaces();
                        for (SIZE_T j = 0; j < Surfaces; ++j)
                        {
                            const FMeshResource& Resource = MeshComponent.StaticMesh->GetMeshResource();
                            if (!Resource.IsSurfaceIndexValid(j))
                            {
                                continue;
                            }
                            
                            const FGeometrySurface& Surface = Resource.GetSurface(j);
                            CMaterialInterface* Material = MeshComponent.GetMaterialForSlot(Surface.MaterialIndex);
                            if (!IsValid(Material) || !Material->IsReadyForRender())
                            {
                                continue;
                            }
                                
                            SIZE_T VertexLocation = MeshVertexOffset[Mesh];
                            SIZE_T StartIndex = MeshIndexOffset[Mesh] + Surface.StartIndex;
                        
                            FStaticMeshRender Proxy;
                            Proxy.Material = Material;
                            Proxy.VertexOffset = (VertexLocation / sizeof(FVertex));
                            Proxy.FirstIndex = (StartIndex / sizeof(uint32));
                            Proxy.Surface = Surface;
                            Proxy.Matrix = TransformComponent.Transform.GetMatrix();

                            uint64 MaterialID = reinterpret_cast<uintptr_t>(Proxy.Material) & 0xFFFFFFFF;
                            Proxy.SortKey = (uint64(MaterialID) << 32) | (uint64(Proxy.VertexOffset) << 16) | (Proxy.FirstIndex & 0xFFFF);

                            StaticMeshRenders.push_back(Proxy);
                        }
                    }
                }
            }

            {
                LUMINA_PROFILE_SECTION("Sort Render Proxies");
                eastl::sort(StaticMeshRenders.begin(), StaticMeshRenders.end());
            }

            {
                LUMINA_PROFILE_SECTION("Build Indirect Draw Arguments");
                
                CMaterial* PrevMaterial = nullptr;
                uint64 PrevSortKey = ~0ull;
                uint32 CurrentInstanceCount = 0;
            
                for (SIZE_T i = 0; i < StaticMeshRenders.size(); ++i)
                {
                    const FStaticMeshRender& Proxy = StaticMeshRenders[i];

                    ModelData.ModelMatrices.push_back(Proxy.Matrix);

                    if (Proxy.SortKey != PrevSortKey)
                    {

                        // New material or sort group, emit a new draw and possibly a new batch
                        if (Proxy.Material->GetMaterial() != PrevMaterial)
                        {
                            FIndirectRenderBatch Batch;
                            Batch.NumDraws = 1;
                            Batch.Material = Proxy.Material;
                            Batch.Offset = IndirectDrawArguments.size();
                            RenderBatches.push_back(Batch);
                        }
                        else
                        {
                            RenderBatches.back().NumDraws++;
                        }

                        FDrawIndexedIndirectArguments DrawArgument;
                        DrawArgument.BaseVertexLocation     = Proxy.VertexOffset;
                        DrawArgument.StartIndexLocation     = Proxy.FirstIndex;
                        DrawArgument.IndexCount             = Proxy.Surface.IndexCount;
                        DrawArgument.StartInstanceLocation  = CurrentInstanceCount;
                        DrawArgument.InstanceCount          = 1;

                        IndirectDrawArguments.push_back(DrawArgument);
                        PrevSortKey = Proxy.SortKey;
                        
                    }
                    else
                    {
                        IndirectDrawArguments.back().InstanceCount++;
                    }


                    CurrentInstanceCount++;
                }
            }

            if (WorkSize != 0)
            {
                LUMINA_PROFILE_SECTION("Write Buffers");
                CommandList->WriteBuffer(ModelDataBuffer, ModelData.ModelMatrices.data(), 0, ModelData.ModelMatrices.size() * sizeof(glm::mat4));
                CommandList->WriteBuffer(IndirectDrawBuffer, IndirectDrawArguments.data(), 0, IndirectDrawArguments.size() * sizeof(FDrawIndexedIndirectArguments));
            }
        }


        //========================================================================================================================
        
        {
            auto Group = Scene->GetMutableEntityRegistry().group<SPointLightComponent>(entt::get<STransformComponent>);

            Group.each([&](auto& PointLightComponent, auto& TransformComponent)
            {
                FLight Light;
                Light.Type = LIGHT_TYPE_POINT;
                
                Light.Color = glm::vec4(PointLightComponent.LightColor, PointLightComponent.Intensity);
                Light.Radius = PointLightComponent.Attenuation;
                Light.Position = glm::vec4(TransformComponent.GetLocation(), 1.0f);
                
                LightData.Lights[LightData.NumLights++] = Memory::Move(Light);
            });
        }

        
        {
            auto Group = Scene->GetMutableEntityRegistry().group<SSpotLightComponent>(entt::get<STransformComponent>);

            Group.each([&](auto& SpotLightComponent, auto& TransformComponent)
            {
                FLight SpotLight;
                SpotLight.Type = LIGHT_TYPE_SPOT;
                
                SpotLight.Position = glm::vec4(TransformComponent.GetLocation(), 1.0f);

                glm::vec3 Forward = TransformComponent.GetRotation() * glm::vec3(0.0f, 0.0f, -1.0f);
                SpotLight.Direction = glm::vec4(glm::normalize(Forward), 0.0f);
                
                SpotLight.Color = glm::vec4(SpotLightComponent.LightColor, SpotLightComponent.Intensity);

                float InnerDegrees = SpotLightComponent.InnerConeAngle;
                float OuterDegrees = SpotLightComponent.OuterConeAngle;

                float InnerCos = glm::cos(glm::radians(InnerDegrees));
                float OuterCos = glm::cos(glm::radians(OuterDegrees));
                
                SpotLight.Angle = glm::vec2(InnerCos, OuterCos);

                SpotLight.Radius = SpotLightComponent.Attenuation;

                LightData.Lights[LightData.NumLights++] = SpotLight;
            });

        }
        

        {
            auto Group = Scene->GetMutableEntityRegistry().group<SDirectionalLightComponent>(entt::get<STransformComponent>);
            Group.each([&](auto& DirectionalLightComponent, auto& TransformComponent)
            {
                FLight DirectionalLight;
                DirectionalLight.Type = LIGHT_TYPE_DIRECTIONAL;
                
                DirectionalLight.Color = glm::vec4(DirectionalLightComponent.Color, DirectionalLightComponent.Intensity);
                glm::vec3 Forward = TransformComponent.GetRotation() * glm::vec3(0.0f, 0.0f, -1.0f);
                DirectionalLight.Direction = glm::vec4(glm::normalize(Forward), 0.0f);
                
                LightData.Lights[LightData.NumLights++] = Memory::Move(DirectionalLight);
            });
        }

        CommandList->WriteBuffer(LightDataBuffer, &LightData, 0, sizeof(FSceneLightData));
    }

    void FSceneRenderer::InitResources()
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
        
        VertexLayoutInput = RenderContext->CreateInputLayout(VertexDesc, std::size(VertexDesc));
        
        FBindingLayoutDesc BindingLayoutDesc;
        BindingLayoutDesc.StageFlags.SetMultipleFlags(ERHIShaderType::Vertex, ERHIShaderType::Fragment);
        BindingLayoutDesc.AddItem(FBindingLayoutItem(0, ERHIBindingResourceType::Buffer_Uniform_Dynamic));
        BindingLayoutDesc.AddItem(FBindingLayoutItem(1, ERHIBindingResourceType::Buffer_SRV));
        BindingLayoutDesc.AddItem(FBindingLayoutItem(2, ERHIBindingResourceType::Buffer_SRV));
        BindingLayoutDesc.AddItem(FBindingLayoutItem(0, ERHIBindingResourceType::PushConstants, 80));
        BindingLayout = RenderContext->CreateBindingLayout(BindingLayoutDesc);

        FBindingSetDesc BindingSetDesc;
        BindingSetDesc.AddItem(FBindingSetItem::BufferCBV(0, SceneDataBuffer));
        BindingSetDesc.AddItem(FBindingSetItem::BufferSRV(1, ModelDataBuffer));
        BindingSetDesc.AddItem(FBindingSetItem::BufferSRV(2, LightDataBuffer));
        
        BindingSet = RenderContext->CreateBindingSet(BindingSetDesc, BindingLayout);

        {
            FBindingLayoutDesc LayoutDesc;
            LayoutDesc.AddItem(FBindingLayoutItem(0, ERHIBindingResourceType::Texture_SRV));
            LayoutDesc.AddItem(FBindingLayoutItem(1, ERHIBindingResourceType::Texture_SRV));
            LayoutDesc.AddItem(FBindingLayoutItem(2, ERHIBindingResourceType::Texture_SRV));
            LayoutDesc.AddItem(FBindingLayoutItem(3, ERHIBindingResourceType::Texture_SRV));
            LayoutDesc.AddItem(FBindingLayoutItem(4, ERHIBindingResourceType::Texture_SRV));
            LayoutDesc.StageFlags.SetMultipleFlags(ERHIShaderType::Fragment);
            
            LightingPassLayout = RenderContext->CreateBindingLayout(LayoutDesc);

            FBindingSetDesc SetDesc;
            SetDesc.AddItem(FBindingSetItem::TextureSRV(0, GBuffer.Position));
            SetDesc.AddItem(FBindingSetItem::TextureSRV(1, GBuffer.Normals));
            SetDesc.AddItem(FBindingSetItem::TextureSRV(2, GBuffer.Material));
            SetDesc.AddItem(FBindingSetItem::TextureSRV(3, GBuffer.AlbedoSpec));
            SetDesc.AddItem(FBindingSetItem::TextureSRV(4, SSAOBlur));

            LightingPassSet = RenderContext->CreateBindingSet(SetDesc, LightingPassLayout);
        }

        {
            FBindingLayoutDesc LayoutDesc;
            LayoutDesc.AddItem(FBindingLayoutItem(0, ERHIBindingResourceType::Texture_SRV));
            LayoutDesc.AddItem(FBindingLayoutItem(1, ERHIBindingResourceType::Texture_SRV));
            LayoutDesc.AddItem(FBindingLayoutItem(2, ERHIBindingResourceType::Texture_SRV));
            LayoutDesc.AddItem(FBindingLayoutItem(3, ERHIBindingResourceType::Buffer_CBV));
            LayoutDesc.StageFlags.SetMultipleFlags(ERHIShaderType::Fragment);
            
            SSAOPassLayout = RenderContext->CreateBindingLayout(LayoutDesc);

            FBindingSetDesc SetDesc;
            SetDesc.AddItem(FBindingSetItem::TextureSRV(0, GBuffer.Position));
            SetDesc.AddItem(FBindingSetItem::TextureSRV(1, GBuffer.Normals));
            SetDesc.AddItem(FBindingSetItem::TextureSRV(2, NoiseImage, GEngine->GetEngineSubsystem<FRenderManager>()->GetNearestSamplerRepeat()));
            SetDesc.AddItem(FBindingSetItem::BufferCBV(2, SSAOKernalBuffer));

            SSAOPassSet = RenderContext->CreateBindingSet(SetDesc, SSAOPassLayout);
        }

        {
            FBindingLayoutDesc LayoutDesc;
            LayoutDesc.AddItem(FBindingLayoutItem(0, ERHIBindingResourceType::Texture_SRV));
            LayoutDesc.StageFlags.SetMultipleFlags(ERHIShaderType::Fragment);
            
            SSAOBlurPassLayout = RenderContext->CreateBindingLayout(LayoutDesc);

            FBindingSetDesc SetDesc;
            SetDesc.AddItem(FBindingSetItem::TextureSRV(0, SSAOImage, GEngine->GetEngineSubsystem<FRenderManager>()->GetNearestSamplerClamped()));

            SSAOBlurPassSet = RenderContext->CreateBindingSet(SetDesc, SSAOBlurPassLayout);
        }
        
        FBindingLayoutItem ImageItem;
        ImageItem.Slot = 0;
        ImageItem.Type = ERHIBindingResourceType::Texture_SRV;

        FBindingLayoutDesc SkyboxLayoutDesc;
        SkyboxLayoutDesc.AddItem(ImageItem);
        SkyboxLayoutDesc.StageFlags.SetFlag(ERHIShaderType::Fragment);
        SkyboxBindingLayout = RenderContext->CreateBindingLayout(SkyboxLayoutDesc);

        FBindingSetDesc SkyboxSetDesc;
        SkyboxSetDesc.AddItem(FBindingSetItem::TextureSRV(0, CubeMap));
        SkyboxBindingSet = RenderContext->CreateBindingSet(SkyboxSetDesc, SkyboxBindingLayout);
    }

    static float lerp(float a, float b, float f)
    {
        return a + f * (b - a);
    }

    void FSceneRenderer::InitBuffers()
    {
        FRHIBufferDesc BufferDesc;
        BufferDesc.Size = sizeof(FSceneGlobalData);
        BufferDesc.Stride = sizeof(FSceneGlobalData);
        BufferDesc.Usage.SetMultipleFlags(BUF_UniformBuffer, BUF_Dynamic);
        BufferDesc.MaxVersions = 3;
        BufferDesc.DebugName = "Scene Global Data";
        SceneDataBuffer = RenderContext->CreateBuffer(BufferDesc);
        RenderContext->SetObjectName(SceneDataBuffer, BufferDesc.DebugName.c_str(), EAPIResourceType::Buffer);

        FRHIBufferDesc ModelBufferDesc;
        ModelBufferDesc.Size = sizeof(glm::mat4) * 100000;
        ModelBufferDesc.Stride = sizeof(glm::mat4);
        ModelBufferDesc.Usage.SetMultipleFlags(BUF_StorageBuffer);
        ModelBufferDesc.bKeepInitialState = true;
        ModelBufferDesc.InitialState = EResourceStates::ShaderResource;
        ModelBufferDesc.DebugName = "Model Buffer";
        ModelDataBuffer = RenderContext->CreateBuffer(ModelBufferDesc);
        RenderContext->SetObjectName(ModelDataBuffer, ModelBufferDesc.DebugName.c_str(), EAPIResourceType::Buffer);
        
        FRHIBufferDesc LightBufferDesc;
        LightBufferDesc.Size = sizeof(FSceneLightData);
        LightBufferDesc.Stride = sizeof(FSceneLightData);
        LightBufferDesc.Usage.SetMultipleFlags(BUF_StorageBuffer);
        LightBufferDesc.bKeepInitialState = true;
        LightBufferDesc.InitialState = EResourceStates::ShaderResource;
        LightBufferDesc.DebugName = "Light Data Buffer";
        LightDataBuffer = RenderContext->CreateBuffer(LightBufferDesc);
        RenderContext->SetObjectName(LightDataBuffer, LightBufferDesc.DebugName.c_str(), EAPIResourceType::Buffer);

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

            FRHICommandListRef CommandList = RenderContext->CreateCommandList(FCommandListInfo::Transfer());
            CommandList->Open();
        
            FRHIBufferDesc SSAOBufferDesc;
            SSAOBufferDesc.Size = SSAOKernel.size() * sizeof(glm::vec4);
            SSAOBufferDesc.Stride = sizeof(glm::vec4);
            SSAOBufferDesc.Usage.SetMultipleFlags(BUF_UniformBuffer);
            SSAOBufferDesc.bKeepInitialState = true;
            SSAOBufferDesc.InitialState = EResourceStates::ShaderResource;
            SSAOBufferDesc.DebugName = "SSAO Kernal Buffer";
            SSAOKernalBuffer = RenderContext->CreateBuffer(SSAOBufferDesc);
            RenderContext->SetObjectName(SSAOKernalBuffer, SSAOBufferDesc.DebugName.c_str(), EAPIResourceType::Buffer);

            CommandList->WriteBuffer(SSAOKernalBuffer, SSAOKernel.data(), 0, SSAOBufferDesc.Size);
            
            CommandList->Close();
            RenderContext->ExecuteCommandList(CommandList, ECommandQueue::Transfer);

            CommandList = RenderContext->CreateCommandList(FCommandListInfo::Graphics());
            CommandList->Open();
            
            FRHIImageDesc SSAONoiseDesc = {};
            SSAONoiseDesc.Extent = {4, 4};
            SSAONoiseDesc.Format = EFormat::RGBA32_FLOAT;
            SSAONoiseDesc.Dimension = EImageDimension::Texture2D;
            SSAONoiseDesc.bKeepInitialState = true;
            SSAONoiseDesc.InitialState = EResourceStates::ShaderResource;
            SSAONoiseDesc.Flags.SetMultipleFlags(EImageCreateFlags::ShaderResource);
            SSAONoiseDesc.DebugName = "SSAO Noise";
        
            NoiseImage = RenderContext->CreateImage(SSAONoiseDesc);
            RenderContext->SetObjectName(NoiseImage, "SSAO Noise", EAPIResourceType::Image);
        
            // Random noise
            TVector<glm::vec4> NoiseValues(32);
            for (SIZE_T i = 0; i < NoiseValues.size(); i++)
            {
                NoiseValues[i] = glm::vec4(rndDist(rndEngine) * 2.0f - 1.0f, rndDist(rndEngine) * 2.0f - 1.0f, 0.0f, 0.0f);
            }
            
            CommandList->WriteImage(NoiseImage, 0, 0, NoiseValues.data(), 4 * 16, 0);

            CommandList->Close();
            RenderContext->ExecuteCommandList(CommandList, ECommandQueue::Graphics);
        }

        
        FRHIBufferDesc VertexBufferDesc;
        VertexBufferDesc.Size = sizeof(FVertex) * (1024 * 1024 * 10);
        VertexBufferDesc.Stride = sizeof(FVertex);
        VertexBufferDesc.Usage.SetMultipleFlags(BUF_VertexBuffer);
        VertexBufferDesc.InitialState = EResourceStates::VertexBuffer;
        VertexBufferDesc.bKeepInitialState = true;
        VertexBufferDesc.DebugName = "Vertex Buffer";
        VertexBuffer = RenderContext->CreateBuffer(VertexBufferDesc);
        RenderContext->SetObjectName(VertexBuffer, VertexBufferDesc.DebugName.c_str(), EAPIResourceType::Buffer);

        
        FRHIBufferDesc IndexBufferDesc;
        IndexBufferDesc.Size = sizeof(uint32) * (1024 * 1024 * 10);
        IndexBufferDesc.Stride = sizeof(uint32);
        IndexBufferDesc.Usage.SetMultipleFlags(BUF_IndexBuffer);
        IndexBufferDesc.InitialState = EResourceStates::IndexBuffer;
        IndexBufferDesc.bKeepInitialState = true;
        IndexBufferDesc.DebugName = "Index Buffer";
        IndexBuffer = RenderContext->CreateBuffer(IndexBufferDesc);
        RenderContext->SetObjectName(IndexBuffer, IndexBufferDesc.DebugName.c_str(), EAPIResourceType::Buffer);

        
        FRHIBufferDesc IndirectBufferDesc;
        IndirectBufferDesc.Size = sizeof(FDrawIndexedIndirectArguments) * (1024 * 1024 * 10);
        IndirectBufferDesc.Stride = sizeof(uint32);
        IndirectBufferDesc.Usage.SetMultipleFlags(BUF_Indirect);
        IndirectBufferDesc.InitialState = EResourceStates::IndexBuffer;
        IndirectBufferDesc.bKeepInitialState = true;
        IndirectBufferDesc.DebugName = "Indirect Draw Buffer";
        IndirectDrawBuffer = RenderContext->CreateBuffer(IndirectBufferDesc);
        RenderContext->SetObjectName(IndexBuffer, IndirectBufferDesc.DebugName.c_str(), EAPIResourceType::Buffer);
    }
    
    void FSceneRenderer::CreateImages()
    {
        FIntVector2D Extent = Windowing::GetPrimaryWindowHandle()->GetExtent();
        
        FRHIImageDesc GBufferPosition;
        GBufferPosition.Extent = Extent;
        GBufferPosition.Flags.SetMultipleFlags(EImageCreateFlags::ColorAttachment, EImageCreateFlags::ShaderResource);
        GBufferPosition.Format = EFormat::RGBA32_FLOAT;
        GBufferPosition.Dimension = EImageDimension::Texture2D;
        GBufferPosition.DebugName = "GBuffer - Position";
        
        GBuffer.Position = RenderContext->CreateImage(GBufferPosition);
        RenderContext->SetObjectName(GBuffer.Position, "GBuffer - Position", EAPIResourceType::Image);

        
        FRHIImageDesc NormalDesc = {};
        NormalDesc.Extent = Extent;
        NormalDesc.Format = EFormat::RGBA16_FLOAT;
        NormalDesc.Dimension = EImageDimension::Texture2D;
        NormalDesc.Flags.SetMultipleFlags(EImageCreateFlags::ColorAttachment, EImageCreateFlags::ShaderResource);
        NormalDesc.DebugName = "GBuffer - Normals";
        
        GBuffer.Normals = RenderContext->CreateImage(NormalDesc);
        RenderContext->SetObjectName(GBuffer.Normals, "GBuffer - Normals", EAPIResourceType::Image);

        
        FRHIImageDesc MaterialDesc = {};
        MaterialDesc.Extent = Extent;
        MaterialDesc.Format = EFormat::RGBA8_UNORM;
        MaterialDesc.Dimension = EImageDimension::Texture2D;
        MaterialDesc.Flags.SetMultipleFlags(EImageCreateFlags::ColorAttachment, EImageCreateFlags::ShaderResource);
        MaterialDesc.DebugName = "GBuffer - Material";
        
        GBuffer.Material = RenderContext->CreateImage(MaterialDesc);
        RenderContext->SetObjectName(GBuffer.Material, "GBuffer - Material", EAPIResourceType::Image);

        
        FRHIImageDesc AlbedoDesc = {};
        AlbedoDesc.Extent = Extent;
        AlbedoDesc.Format = EFormat::RGBA8_UNORM;
        AlbedoDesc.Dimension = EImageDimension::Texture2D;
        AlbedoDesc.Flags.SetMultipleFlags(EImageCreateFlags::ColorAttachment, EImageCreateFlags::ShaderResource);
        AlbedoDesc.DebugName = "GBuffer - Albedo";
        
        GBuffer.AlbedoSpec = RenderContext->CreateImage(AlbedoDesc);
        RenderContext->SetObjectName(GBuffer.AlbedoSpec, "GBuffer - Albedo", EAPIResourceType::Image);
        
        
        FRHIImageDesc DepthImageDesc;
        DepthImageDesc.Extent = Extent;
        DepthImageDesc.Flags.SetMultipleFlags(EImageCreateFlags::DepthAttachment, EImageCreateFlags::ShaderResource);
        DepthImageDesc.Format = EFormat::D32;
        DepthImageDesc.InitialState = EResourceStates::DepthWrite;
        DepthImageDesc.bKeepInitialState = true;
        DepthImageDesc.Dimension = EImageDimension::Texture2D;
        DepthImageDesc.DebugName = "Depth Attachment";

        DepthAttachment = RenderContext->CreateImage(DepthImageDesc);
        RenderContext->SetObjectName(DepthAttachment, "Depth Attachment", EAPIResourceType::Image);

        {
            FRHIImageDesc SSAODesc = {};
            SSAODesc.Extent = Extent;
            SSAODesc.Format = EFormat::R8_UNORM;
            SSAODesc.Dimension = EImageDimension::Texture2D;
            SSAODesc.Flags.SetMultipleFlags(EImageCreateFlags::ColorAttachment, EImageCreateFlags::ShaderResource);
            SSAODesc.DebugName = "SSAO";
        
            SSAOImage = RenderContext->CreateImage(SSAODesc);
            RenderContext->SetObjectName(SSAOImage, "SSAO", EAPIResourceType::Image);
        }

        {
            FRHIImageDesc SSAODesc = {};
            SSAODesc.Extent = Extent;
            SSAODesc.Format = EFormat::R8_UNORM;
            SSAODesc.Dimension = EImageDimension::Texture2D;
            SSAODesc.Flags.SetMultipleFlags(EImageCreateFlags::ColorAttachment, EImageCreateFlags::ShaderResource);
            SSAODesc.DebugName = "SSAO Blur";
        
            SSAOBlur = RenderContext->CreateImage(SSAODesc);
            RenderContext->SetObjectName(SSAOBlur, "SSAO Blur", EAPIResourceType::Image);
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

            CubeMap = RenderContext->CreateImage(SkyCubeMapDesc);
            RenderContext->SetObjectName(CubeMap, "CubeMap", EAPIResourceType::Image);

            static const char* CubeFaceFiles[6] = {
                "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg"
            };

            FRHICommandListRef CommandList = RenderContext->CreateCommandList(FCommandListInfo::Transfer());
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
            RenderContext->ExecuteCommandList(CommandList, ECommandQueue::Transfer);
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

            DepthMap = RenderContext->CreateImage(DepthMapDesc);
            RenderContext->SetObjectName(DepthMap, "Depth Map", EAPIResourceType::Image);
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

            ShadowCubeMap = RenderContext->CreateImage(CubeMapDesc);
            RenderContext->SetObjectName(ShadowCubeMap, CubeMapDesc.DebugName.c_str(), EAPIResourceType::Image);
        }
        
    }
    
}
