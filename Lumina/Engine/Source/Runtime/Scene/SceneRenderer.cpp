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

#include "RenderProxy.h"
#include "Assets/AssetTypes/Material/Material.h"
#include "Assets/AssetTypes/Textures/Texture.h"
#include "glm/gtx/quaternion.hpp"
#include "Core/Engine/Engine.h"
#include "Core/Profiler/Profile.h"
#include "Entity/Entity.h"
#include "Entity/Components/LightComponent.h"
#include "Entity/Components/StaticMeshComponent.h"
#include "Paths/Paths.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"
#include "Renderer/ShaderCompiler.h"
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
        RenderContext = GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext();
        LOG_TRACE("Initializing Scene Renderer");

        Scene->GetMutableEntityRegistry().on_update<SStaticMeshComponent>().connect<&FSceneRenderer::OnStaticMeshComponentCreated>(this);
        Scene->GetMutableEntityRegistry().on_construct<SStaticMeshComponent>().connect<&FSceneRenderer::OnStaticMeshComponentCreated>(this);
        Scene->GetMutableEntityRegistry().on_destroy<SStaticMeshComponent>().connect<&FSceneRenderer::OnStaticMeshComponentDestroyed>(this);
        
        // Wait for shader tasks.
        while (RenderContext->GetShaderCompiler()->HasPendingRequests()) {}
        
        InitBuffers();
        CreateImages();
        //CreateIrradianceCube();
        InitResources();

        AddRenderPass("Geometry Pass", [this]()
        {
            LUMINA_PROFILE_SCOPE_COLORED(tracy::Color::Red);
        
            ICommandList* CommandList = RenderContext->GetCommandList(ECommandQueue::Graphics);
            
            if (RenderBatches.empty())
            {
                return;
            }
            
            CommandList->SetBufferState(IndirectDrawBuffer , EResourceStates::IndirectArgument);
            CommandList->CommitBarriers();
            
            
            FRenderPassBeginInfo BeginInfo; BeginInfo
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
            .SetDepthClearValue(0.0f)
            .SetDepthLoadOp(ERenderLoadOp::Clear)
            .SetDepthStoreOp(ERenderStoreOp::Store)        
                
            .SetRenderArea(GetRenderTarget()->GetExtent());
            BeginInfo.DebugName = "Geometry Pass";
            CommandList->BeginRenderPass(BeginInfo);
            
            float SizeY = (float)GBuffer.AlbedoSpec->GetSizeY();
            float SizeX = (float)GBuffer.AlbedoSpec->GetSizeX();
            CommandList->SetViewport(0.0f, 0.0f, 0.0f, SizeX, SizeY, 1.0f);
            CommandList->SetScissorRect(0, 0, SizeX, SizeY);
            
            FDepthStencilState DepthState;
            DepthState.SetDepthFunc(EComparisonFunc::Greater);
            
            FBlendState BlendState;
            FBlendState::RenderTarget AlbedoTarget;
            AlbedoTarget.DisableBlend();
            AlbedoTarget.SetFormat(EFormat::RGBA16_FLOAT);
            BlendState.SetRenderTarget(0, AlbedoTarget);
            
            FBlendState::RenderTarget NormalTarget;
            NormalTarget.DisableBlend();
            NormalTarget.SetFormat(EFormat::RGBA16_FLOAT);
            BlendState.SetRenderTarget(1, NormalTarget);
            
            FBlendState::RenderTarget MaterialTarget;
            MaterialTarget.DisableBlend();
            MaterialTarget.SetFormat(EFormat::RGBA8_UNORM);
            BlendState.SetRenderTarget(2, MaterialTarget);
            
            FBlendState::RenderTarget AlbedoSpecTarget;
            AlbedoSpecTarget.DisableBlend();
            AlbedoSpecTarget.SetFormat(EFormat::RGBA8_UNORM);
            BlendState.SetRenderTarget(3, AlbedoSpecTarget);
            
            FRasterState RasterState;
            RasterState.SetDepthClipEnable(true);
                
            FRenderState RenderState;
            RenderState.SetBlendState(BlendState);
            RenderState.SetRasterState(RasterState);
            RenderState.SetDepthStencilState(DepthState);
            
            CommandList->BindVertexBuffer(VertexBuffer, 0, 0);
            SceneRenderStats.NumVertices = Vertices.size();
            SceneRenderStats.NumIndices = Indices.size();
            
            for (SIZE_T CurrentDraw = 0; CurrentDraw < RenderBatches.size(); ++CurrentDraw)
            {
                const FIndirectRenderBatch& Batch = RenderBatches[CurrentDraw];
                
                CMaterialInterface* Material = Batch.Material;
            
                FGraphicsPipelineDesc Desc;
                Desc.SetRenderState(RenderState);
                Desc.SetInputLayout(VertexLayoutInput);
                Desc.AddBindingLayout(BindingLayout);
                Desc.AddBindingLayout(Material->GetBindingLayout());
                Desc.SetVertexShader(Material->GetMaterial()->VertexShader);
                Desc.SetPixelShader(Material->GetMaterial()->PixelShader);
                
                FRHIGraphicsPipelineRef Pipeline = RenderContext->CreateGraphicsPipeline(Desc);
                
                CommandList->SetGraphicsPipeline(Pipeline);
            
                CommandList->BindBindingSets(ERHIBindingPoint::Graphics, {{BindingSet, 0}, {Material->GetBindingSet(), 1}});

                SceneRenderStats.NumDrawCalls += Batch.NumDraws;
                CommandList->DrawIndexedIndirect(IndirectDrawBuffer, IndexBuffer, Batch.NumDraws, Batch.Offset * sizeof(FDrawIndexedIndirectArguments));
            }
            
            CommandList->EndRenderPass();
        });

        AddRenderPass("Shadow Pass", [this]
        {

            
            
        });

        AddRenderPass("Lighting Pass", [this] ()
        {
            LUMINA_PROFILE_SCOPE_COLORED(tracy::Color::Blue);
            FRHIVertexShaderRef VertexShader = RenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("DeferredLighting.vert");
            FRHIPixelShaderRef PixelShader = RenderContext->GetShaderLibrary()->GetShader<FRHIPixelShader>("DeferredLighting.frag");
            if (!VertexShader || !PixelShader)
            {
                return;
            }
            
            ICommandList* CommandList = RenderContext->GetCommandList(Q_Graphics);
            
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
                
            CommandList->SetGraphicsPipeline(Pipeline);
            CommandList->BindBindingSets(ERHIBindingPoint::Graphics, {{BindingSet, 0}, {LightingPassSet, 1}});
            
            FRenderPassBeginInfo BeginInfo; BeginInfo
            .AddColorAttachment(GetRenderTarget())
            .SetColorLoadOp(ERenderLoadOp::Load)
            .SetColorStoreOp(ERenderStoreOp::Store)
            .SetColorClearColor(FColor::Black)
    
            .SetDepthAttachment(DepthAttachment)
            .SetDepthLoadOp(ERenderLoadOp::Load)
            .SetDepthStoreOp(ERenderStoreOp::Store)
            
            .SetRenderArea(GetRenderTarget()->GetExtent());
            BeginInfo.DebugName = "Lighting Pass";
            CommandList->BeginRenderPass(BeginInfo);
            
                
            float SizeY = (float)GBuffer.AlbedoSpec->GetSizeY();
            float SizeX = (float)GBuffer.AlbedoSpec->GetSizeX();
            CommandList->SetViewport(0.0f, 0.0f, 0.0f, SizeX, SizeY, 1.0f);
            CommandList->SetScissorRect(0, 0, SizeX, SizeY);

            SceneRenderStats.NumDrawCalls++;
            SceneRenderStats.NumVertices += 3;
            CommandList->Draw(3, 1, 0, 0);
    
            CommandList->EndRenderPass(); 
        });

        AddRenderPass("Skybox Pass", [this]()
        {
            LUMINA_PROFILE_SCOPE_COLORED(tracy::Color::Green);

            ICommandList* CommandList = RenderContext->GetCommandList(ECommandQueue::Graphics);
            
            FRHIVertexShaderRef VertexShader = RenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("Skybox.vert");
            FRHIPixelShaderRef PixelShader = RenderContext->GetShaderLibrary()->GetShader<FRHIPixelShader>("Skybox.frag");
            if (!VertexShader || !PixelShader)
            {
                return;
            }
            
            FRHIImageRef ColorAttachment = GetRenderTarget();
            
            FDepthStencilState DepthState;
            DepthState.SetDepthTestEnable(true);
            DepthState.SetDepthWriteEnable(false);
            DepthState.SetDepthFunc(EComparisonFunc::GreaterOrEqual);
            
            FBlendState BlendState;
            FBlendState::RenderTarget RenderTarget;
            RenderTarget.DisableBlend();
            RenderTarget.SetFormat(EFormat::BGRA8_UNORM);
            BlendState.SetRenderTarget(0, RenderTarget);
            
            FRasterState RasterState;
            RasterState.SetCullFront();
            RasterState.SetDepthClipEnable(false);
            
            FRenderState RenderState;
            RenderState.SetBlendState(BlendState);
            RenderState.SetRasterState(RasterState);
            RenderState.SetDepthStencilState(DepthState);
            
            FGraphicsPipelineDesc Desc;
            Desc.SetRenderState(RenderState);
            Desc.AddBindingLayout(BindingLayout);
            Desc.AddBindingLayout(SkyboxBindingLayout);
            Desc.SetVertexShader(VertexShader);
            Desc.SetPixelShader(PixelShader);
            
            FRHIGraphicsPipelineRef Pipeline = RenderContext->CreateGraphicsPipeline(Desc);
            
            CommandList->SetGraphicsPipeline(Pipeline);
            
            CommandList->BindBindingSets(ERHIBindingPoint::Graphics, {{SkyboxBindingSet, 1}});
            
            FRenderPassBeginInfo BeginInfo; BeginInfo
            .AddColorAttachment(ColorAttachment)
            .SetColorLoadOp(ERenderLoadOp::Load)
            .SetColorStoreOp(ERenderStoreOp::Store)
            .SetColorClearColor(FColor::Black)
                    
            .SetDepthAttachment(DepthAttachment)
            .SetDepthLoadOp(ERenderLoadOp::Load)
            .SetDepthStoreOp(ERenderStoreOp::Store)
                    
            .SetRenderArea(ColorAttachment->GetExtent());
            BeginInfo.DebugName = "Skybox Pass";
            
            CommandList->BeginRenderPass(BeginInfo);
            
            CommandList->SetViewport(0.0f, 0.0f, 0.0f, (float)ColorAttachment->GetSizeX(), (float)ColorAttachment->GetSizeY(), 1.0f);
            CommandList->SetScissorRect(0, 0, ColorAttachment->GetSizeX(), ColorAttachment->GetSizeY());

            SceneRenderStats.NumDrawCalls++;
            SceneRenderStats.NumVertices += 36;
            CommandList->Draw(36, 1, 0, 0);
            
            CommandList->EndRenderPass();
        });

        AddRenderPass("Debug Primitives", [this]()
        {
            LUMINA_PROFILE_SCOPE();

            ICommandList* CommandList = RenderContext->GetCommandList(Q_Graphics);
            
            FRHIVertexShaderRef VertexShader = RenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("Simple.vert");
            FRHIPixelShaderRef PixelShader = RenderContext->GetShaderLibrary()->GetShader<FRHIPixelShader>("Simple.frag");
            if (!VertexShader || !PixelShader)
            {
                return;
            }
            
            FRenderPassBeginInfo BeginInfo; BeginInfo
            .AddColorAttachment(GetRenderTarget())
            .SetColorLoadOp(ERenderLoadOp::Load)
            .SetColorStoreOp(ERenderStoreOp::Store)
            
            .SetDepthAttachment(DepthAttachment)
            .SetDepthLoadOp(ERenderLoadOp::Load)
            .SetDepthStoreOp(ERenderStoreOp::Store)        
                
            .SetRenderArea(GetRenderTarget()->GetExtent());
            BeginInfo.DebugName = "Primitive Pass";
            CommandList->BeginRenderPass(BeginInfo);
            
            FDepthStencilState DepthState;
            DepthState.SetDepthFunc(EComparisonFunc::Greater);
            
            FBlendState BlendState;
            FBlendState::RenderTarget RenderTarget;
            RenderTarget.DisableBlend();
            RenderTarget.SetSrcBlend(EBlendFactor::SrcAlpha);
            RenderTarget.SetDestBlend(EBlendFactor::OneMinusSrcAlpha);
            RenderTarget.SetSrcBlendAlpha(EBlendFactor::One);
            RenderTarget.SetDestBlendAlpha(EBlendFactor::Zero);
            RenderTarget.SetFormat(EFormat::BGRA8_UNORM);
            BlendState.SetRenderTarget(0, RenderTarget);
            
            FRasterState RasterState;
            RasterState.SetDepthClipEnable(true);
                
            FRenderState RenderState;
            RenderState.SetBlendState(BlendState);
            RenderState.SetRasterState(RasterState);
            RenderState.SetDepthStencilState(DepthState);
                
            FGraphicsPipelineDesc Desc;
            Desc.SetRenderState(RenderState);
            Desc.AddBindingLayout(BindingLayout);
            Desc.SetVertexShader(VertexShader);
            Desc.SetPixelShader(PixelShader);
                
            FRHIGraphicsPipelineRef Pipeline = RenderContext->CreateGraphicsPipeline(Desc);
                
            CommandList->SetGraphicsPipeline(Pipeline);
            
            for (uint32 i = 0; i < LightData.NumLights; ++i)
            {
                FLight Light = LightData.Lights[i];
            
                struct FPC
                {
                    glm::mat4 ModelMatrix;
                    glm::vec4 Color;
                } PC;
            
                FTransform Transform;
                Transform.Location = Light.Position;
                PC.ModelMatrix = Transform.GetMatrix();
                PC.ModelMatrix = glm::scale(PC.ModelMatrix, glm::vec3(0.15f));
            
                PC.Color = Light.Color;
            
                float SizeY = (float)GBuffer.AlbedoSpec->GetSizeY();
                float SizeX = (float)GBuffer.AlbedoSpec->GetSizeX();
                CommandList->SetViewport(0.0f, 0.0f, 0.0f, SizeX, SizeY, 1.0f);
                CommandList->SetScissorRect(0, 0, SizeX, SizeY);
            
                CommandList->SetPushConstants(&PC, sizeof(FPC));

                SceneRenderStats.NumDrawCalls++;
                SceneRenderStats.NumVertices += 36;
                CommandList->Draw(36, 1, 0, 0);
            }
            
            CommandList->EndRenderPass();
        });
    }

    void FSceneRenderer::Deinitialize()
    {
        LOG_TRACE("Shutting down scene renderer");

        for (FRenderPass* Pass : RenderPasses)
        {
            Memory::Delete(Pass);
        }
        RenderPasses.clear();
    }

    void FSceneRenderer::StartScene(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();
    }

    void FSceneRenderer::EndScene(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();
        SceneRenderStats = {};

        ICommandList* CommandList = RenderContext->GetCommandList(Q_Graphics);
        
        FCameraManager* CameraManager = Scene->GetSceneSubsystem<FCameraManager>();
        SCameraComponent& CameraComponent = CameraManager->GetActiveCameraEntity().GetComponent<SCameraComponent>();

        SceneGlobalData.CameraData.Location =   glm::vec4(CameraComponent.GetPosition(), 1.0f);
        SceneGlobalData.CameraData.View =       CameraComponent.GetViewMatrix();
        SceneGlobalData.CameraData.Projection = CameraComponent.GetProjectionMatrix();
        SceneGlobalData.Time =                  (float)Scene->GetTimeSinceSceneCreation();
        SceneGlobalData.DeltaTime =             (float)Scene->GetSceneDeltaTime(); 
        
        SceneViewport->SetViewVolume(CameraComponent.GetViewVolume());

        CommandList->WriteBuffer(SceneDataBuffer, &SceneGlobalData, 0, sizeof(FSceneGlobalData));
        
        BuildPasses();
        ExecutePasses();
        
        CommandList->SetImageState(GetRenderTarget(), AllSubresources, EResourceStates::ShaderResource);
        CommandList->CommitBarriers();
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
    
    void FSceneRenderer::AddRenderPass(const FName& Name, FRenderPassFunction&& Function)
    {
        FRenderPass* Pass = Memory::New<FRenderPass>(Name);
        RenderPasses.push_back(Pass);
        Pass->SetPassFunc(std::move(Function));
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

    void FSceneRenderer::ExecutePasses()
    {
        for (FRenderPass* Pass : RenderPasses)
        {
            Pass->Execute();
        }
    }

    void FSceneRenderer::FullScreenPass(const FScene* Scene)
    {
#if 0
        IRenderContext* RenderContext = GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext();
        ICommandList* CommandList = RenderContext->GetCommandList(Q_Graphics);
        
        FRHIVertexShaderRef VertexShader = RenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("InfiniteGrid.vert");
        FRHIPixelShaderRef PixelShader = RenderContext->GetShaderLibrary()->GetShader<FRHIPixelShader>("InfiniteGrid.frag");
        if (!VertexShader || !PixelShader)
        {
            return;
        }
        
        FBindingLayoutItem Item;
        Item.Size = sizeof(FSceneGlobalData);
        Item.Slot = 0;
        Item.Type = ERHIBindingResourceType::Buffer_CBV;

        FBindingLayoutDesc LayoutDesc;
        LayoutDesc.AddItem(Item);
        LayoutDesc.StageFlags.SetFlag(ERHIShaderType::Vertex);
        FRHIBindingLayoutRef Layout = RenderContext->CreateBindingLayout(LayoutDesc);

        FBindingSetDesc SetDesc;
        SetDesc.AddItem(FBindingSetItem::BufferCBV(0, SceneDataBuffer));
        FRHIBindingSetRef Set = RenderContext->CreateBindingSet(SetDesc, Layout);

        FRasterState RasterState;
        RasterState.SetCullNone();

        FBlendState BlendState;
        FBlendState::RenderTarget RenderTarget;
        RenderTarget.EnableBlend();
        RenderTarget.SetSrcBlend(EBlendFactor::SrcAlpha);
        RenderTarget.SetDestBlend(EBlendFactor::OneMinusSrcAlpha);
        RenderTarget.SetSrcBlendAlpha(EBlendFactor::Zero);
        RenderTarget.SetDestBlendAlpha(EBlendFactor::One);
        BlendState.SetRenderTarget(0, RenderTarget);

        FDepthStencilState DepthState;
        DepthState.DisableDepthTest();
        
        FRenderState RenderState;
        RenderState.SetRasterState(RasterState);
        RenderState.SetDepthStencilState(DepthState);
        RenderState.SetBlendState(BlendState);
        
        FGraphicsPipelineDesc Desc;
        Desc.SetRenderState(RenderState);
        Desc.AddBindingLayout(Layout);
        Desc.SetVertexShader(VertexShader);
        Desc.SetPixelShader(PixelShader);

        FRHIGraphicsPipelineRef Pipeline = RenderContext->CreateGraphicsPipeline(Desc);

        CommandList->SetGraphicsPipeline(Pipeline);
        CommandList->BindBindingSets({Set}, ERHIBindingPoint::Graphics);
        
        FRenderPassBeginInfo BeginInfo; BeginInfo
        .AddColorAttachment(GetRenderTarget())
        .SetDepthAttachment(DepthBuffer)
        .SetColorLoadOp(ERenderLoadOp::Clear)
        .SetColorStoreOp(ERenderStoreOp::Store)
        .SetDepthLoadOp(ERenderLoadOp::Clear)
        .SetDepthStoreOp(ERenderStoreOp::Store)
        .SetRenderArea(GetRenderTarget()->GetExtent())
        .SetColorClearColor(FColor::Black);
        CommandList->BeginRenderPass(BeginInfo);

        CommandList->SetViewport(0.0f, 0.0f, 0.0f, (float)GetRenderTarget()->GetSizeX(), (float)GetRenderTarget()->GetSizeY(), 1.0f);
        CommandList->SetScissorRect(0, 0, GetRenderTarget()->GetSizeX(), GetRenderTarget()->GetSizeY());

        CommandList->Draw(6, 1, 0, 0);
        
        CommandList->EndRenderPass();
#endif
    }


    // https://github.com/SaschaWillems/Vulkan/blob/master/examples/pbrtexture/pbrtexture.cpp
    void FSceneRenderer::CreateIrradianceCube()
    {
        FRHIVertexShaderRef VertexShader = RenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("FilterCube.vert");
        FRHIPixelShaderRef PixelShader = RenderContext->GetShaderLibrary()->GetShader<FRHIPixelShader>("IrradianceCube.frag");
        if (!VertexShader || !PixelShader)
        {
            Threading::Sleep(100);
            CreateIrradianceCube();
            return;
        }
    
        const uint32 Dim = 64;
        const uint32 NumMips = static_cast<uint32>(floor(log2(Dim))) + 1;
        
        FRHIImageDesc IrradianceImageDesc;
        IrradianceImageDesc.Extent = {Dim, Dim};
        IrradianceImageDesc.Flags.SetFlag(EImageCreateFlags::CubeCompatible);
        IrradianceImageDesc.Flags.SetFlag(EImageCreateFlags::ShaderResource);
        IrradianceImageDesc.Format = EFormat::RGBA32_FLOAT;
        IrradianceImageDesc.Dimension = EImageDimension::Texture2D;
        IrradianceImageDesc.ArraySize = 6;
        IrradianceImageDesc.NumMips = 1;//(uint32)NumMips;
        IrradianceImageDesc.DebugName = "Irradiance Cube";

        IrradianceCube = RenderContext->CreateImage(IrradianceImageDesc);

        FRHIImageDesc Desc;
        Desc.Format = EFormat::BGRA8_UNORM;
        Desc.Flags.SetMultipleFlags(EImageCreateFlags::RenderTarget, EImageCreateFlags::ShaderResource);
        Desc.Extent.X = Dim;
        Desc.Extent.Y = Dim;
        Desc.DebugName = "Temporary RT";

        FRHIImageRef TemporaryRT = RenderContext->CreateImage(Desc);
        
        TVector<glm::mat4> matrices =
        {
            // POSITIVE_X
            glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
            // NEGATIVE_X
            glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
            // POSITIVE_Y
            glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
            // NEGATIVE_Y
            glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
            // POSITIVE_Z
            glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
            // NEGATIVE_Z
            glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        };

        FBindingLayoutDesc BindingLayoutDesc;
        BindingLayoutDesc.StageFlags.SetMultipleFlags(ERHIShaderType::Vertex, ERHIShaderType::Fragment);
        BindingLayoutDesc.AddItem(FBindingLayoutItem(0, ERHIBindingResourceType::Texture_SRV));
        BindingLayoutDesc.AddItem(FBindingLayoutItem(0, ERHIBindingResourceType::PushConstants, 72));
        FRHIBindingLayoutRef Layout = RenderContext->CreateBindingLayout(BindingLayoutDesc);

        FBindingSetDesc BindingSetDesc;
        BindingSetDesc.AddItem(FBindingSetItem::TextureSRV(0, CubeMap));
        
        FRHIBindingSetRef Set = RenderContext->CreateBindingSet(BindingSetDesc, Layout);

        ICommandList* CommandList = RenderContext->CreateCommandList(FCommandListInfo::Graphics());
        CommandList->Open();

        // Pipeline layout
        struct PushBlock {
            glm::mat4 mvp;
            // Sampling deltas
            float deltaPhi = (2.0f * float(3.14159265358979323846)) / 180.0f;
            float deltaTheta = (0.5f * float(3.14159265358979323846)) / 64.0f;
        } PushBlock;
        
        for (SIZE_T Mip = 0; Mip < NumMips; ++Mip)
        {
            for (SIZE_T Face = 0; Face < 6; ++Face)
            {
                float Width = static_cast<float>(Dim * std::pow(0.5f, Mip));
                float Height = static_cast<float>(Dim * std::pow(0.5f, Mip));
                
                FRHIImageRef ColorAttachment = GetRenderTarget();
        
                FDepthStencilState DepthState;
                DepthState.SetDepthTestEnable(true);
                DepthState.SetDepthWriteEnable(false);
                DepthState.SetDepthFunc(EComparisonFunc::GreaterOrEqual);
                
                FBlendState BlendState;
                FBlendState::RenderTarget RenderTarget;
                RenderTarget.DisableBlend();
                RenderTarget.SetFormat(EFormat::BGRA8_UNORM);
                BlendState.SetRenderTarget(0, RenderTarget);
                
                FRasterState RasterState;
                RasterState.SetCullFront();
                RasterState.SetDepthClipEnable(false);
                
                FRenderState RenderState;
                RenderState.SetBlendState(BlendState);
                RenderState.SetRasterState(RasterState);
                RenderState.SetDepthStencilState(DepthState);
                
                FGraphicsPipelineDesc PipelineDesc;
                PipelineDesc.SetRenderState(RenderState);
                PipelineDesc.AddBindingLayout(Layout);
                PipelineDesc.SetVertexShader(VertexShader);
                PipelineDesc.SetPixelShader(PixelShader);
                
                FRHIGraphicsPipelineRef Pipeline = RenderContext->CreateGraphicsPipeline(PipelineDesc);
                
                CommandList->SetGraphicsPipeline(Pipeline);
                
                CommandList->BindBindingSets(ERHIBindingPoint::Graphics, {{Set, 1}});
                
                FRenderPassBeginInfo BeginInfo; BeginInfo
                .AddColorAttachment(TemporaryRT)
                .SetColorLoadOp(ERenderLoadOp::Load)
                .SetColorStoreOp(ERenderStoreOp::Store)
                .SetColorClearColor(FColor::Black)
                
                .SetRenderArea({Dim, Dim});
                CommandList->BeginRenderPass(BeginInfo);
                
                CommandList->SetViewport(0.0f, 0.0f, 0.0f, Width, Height, 1.0f);
                CommandList->SetScissorRect(0, 0, Dim, Dim);

                CommandList->SetPushConstants(&PushBlock, sizeof(struct PushBlock));

                SceneRenderStats.NumDrawCalls++;
                SceneRenderStats.NumVertices += 36;
                CommandList->Draw(36, 1, 0, 0);
                
                CommandList->EndRenderPass();

                FTextureSlice Src;
                FTextureSlice Dst;
                Dst.MipLevel = 0;
                Dst.ArraySlice = Face;
                
                CommandList->CopyImage(TemporaryRT, Src, IrradianceCube, Dst);
            }
        }

        CommandList->Close();
        RenderContext->ExecuteCommandList(CommandList, ECommandQueue::Transfer);
        
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

        
        FBindingLayoutDesc LayoutDesc;
        LayoutDesc.AddItem(FBindingLayoutItem(0, ERHIBindingResourceType::Texture_SRV));
        LayoutDesc.AddItem(FBindingLayoutItem(1, ERHIBindingResourceType::Texture_SRV));
        LayoutDesc.AddItem(FBindingLayoutItem(2, ERHIBindingResourceType::Texture_SRV));
        LayoutDesc.AddItem(FBindingLayoutItem(3, ERHIBindingResourceType::Texture_SRV));
        LayoutDesc.StageFlags.SetMultipleFlags(ERHIShaderType::Fragment);
        LightingPassLayout = RenderContext->CreateBindingLayout(LayoutDesc);

        FBindingSetDesc SetDesc;
        SetDesc.AddItem(FBindingSetItem::TextureSRV(0, GBuffer.Position));
        SetDesc.AddItem(FBindingSetItem::TextureSRV(1, GBuffer.Normals));
        SetDesc.AddItem(FBindingSetItem::TextureSRV(2, GBuffer.Material));
        SetDesc.AddItem(FBindingSetItem::TextureSRV(3, GBuffer.AlbedoSpec));

        LightingPassSet = RenderContext->CreateBindingSet(SetDesc, LightingPassLayout);

        
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

    void FSceneRenderer::InitBuffers()
    {
        ICommandList* CommandList = RenderContext->GetCommandList(ECommandQueue::Graphics);
        
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
        GBufferPosition.Format = EFormat::RGBA16_FLOAT;
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

            ICommandList* CommandList = RenderContext->CreateCommandList(FCommandListInfo::Transfer());
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
