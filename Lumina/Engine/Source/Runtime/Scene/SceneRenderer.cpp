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

#include "Assets/AssetTypes/Textures/Texture.h"
#include "glm/gtx/quaternion.hpp"
#include "Core/Engine/Engine.h"
#include "Core/Profiler/Profile.h"
#include "Entity/Entity.h"
#include "Entity/Components/LightComponent.h"
#include "Entity/Components/StaicMeshComponent.h"
#include "Paths/Paths.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"
#include "Rendering/RenderGraphResources.h"
#include "Rendering/SkyboxRenderPass.h"
#include "Subsystems/FCameraManager.h"
#include "TaskSystem/TaskSystem.h"
#include "Tools/Import/ImportHelpers.h"

namespace Lumina
{
    
    FSceneRenderer::FSceneRenderer(FScene* InScene)
        : Scene(InScene)
        , SceneGlobalData()
        , RenderGraph(GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext())
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

        // Wait for shader tasks.
        FTaskSystem::Get()->WaitForAll();
        
        InitBuffers();
        CreateImages();
        //CreateIrradianceCube();
        InitResources();

        RenderGraph.AddResource<PrimaryRenderTargetTag>(GetRenderTarget());
        RenderGraph.AddResource<DepthBufferTargetTag>(DepthAttachment);
        RenderGraph.AddResource<SceneGlobalDataTag>(SceneDataBuffer);
        RenderGraph.AddResource<CubeMapImageTag>(CubeMap);
    }

    void FSceneRenderer::Deinitialize()
    {
        RenderGraph.ClearPasses();
    }

    void FSceneRenderer::StartScene(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();
    }

    void FSceneRenderer::EndScene(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();

        ICommandList* CommandList = RenderContext->GetCommandList(Q_Graphics);
        uint32 FrameIndex = UpdateContext.GetSubsystem<FRenderManager>()->GetCurrentFrameIndex();
        SceneRenderStats = {};
        
        FCameraManager* CameraManager = Scene->GetSceneSubsystem<FCameraManager>();
        FCameraComponent& CameraComponent = CameraManager->GetActiveCameraEntity().GetComponent<FCameraComponent>();

        SceneGlobalData.CameraData.Location =   glm::vec4(CameraComponent.GetPosition(), 1.0f);
        SceneGlobalData.CameraData.View =       CameraComponent.GetViewMatrix();
        SceneGlobalData.CameraData.Projection = CameraComponent.GetProjectionMatrix();
        SceneGlobalData.Time =                  (float)Scene->GetTimeSinceSceneCreation();
        SceneGlobalData.DeltaTime =             (float)Scene->GetSceneDeltaTime(); 
        
        SceneViewport->SetViewVolume(CameraComponent.GetViewVolume());
        
        FSceneRenderData DrawList;
        CombinedVertex.clear();
        CombinedIndex.clear();
        
        {
            LUMINA_PROFILE_SECTION("Build Light List");
            auto PointLightGroup = Scene->GetMutableEntityRegistry().group<>(entt::get<FTransformComponent, FPointLightComponent>);
            PointLightGroup.each([&](auto& TransformComponent, auto& PointLightComponent)
            {
                FPointLight PointLight;
                PointLight.Position = glm::vec4(TransformComponent.GetLocation(), 1.0f);
                PointLight.Color = PointLightComponent.LightColor;
                DrawList.LightData.PointLightsLights[DrawList.LightData.NumPointLights] = PointLight;
                            
                DrawList.LightData.NumPointLights++;
            });

            auto DirectionalLightGroup = Scene->GetMutableEntityRegistry().group<>(entt::get<FDirectionalLightComponent>);
            DirectionalLightGroup.each([&] (auto& DirectionalLightComponent)
            {
                FDirectionalLight DirectionalLight;
                DirectionalLight.Direction = DirectionalLightComponent.Direction;
                DirectionalLight.Color = DirectionalLightComponent.Color;
                DrawList.LightData.bHasDirectionalLight = true;
                DrawList.LightData.DirectionalLight = DirectionalLight;
            });
        }



        
        TVector<SIZE_T> DirtyRenderProxies;
        TVector<CStaticMesh*> NewMeshes;
        THashMap<CStaticMesh*, SIZE_T> MeshCountMap;

        {
            auto DirtyGroup = Scene->GetMutableEntityRegistry().view<FNeedsRenderProxyUpdate, FStaticMeshComponent, FTransformComponent>();
            for (const auto& DirtyEntity : DirtyGroup)
            {
                Entity Entity(DirtyEntity, Scene);
                FStaticMeshComponent& MeshComponent = Entity.GetComponent<FStaticMeshComponent>();
                FTransformComponent& TransformComponent = Entity.GetComponent<FTransformComponent>();

                CStaticMesh* Mesh = MeshComponent.StaticMesh;
                if (!IsValid(Mesh) || !Mesh->IsReadyForRender())
                {
                    continue;
                }

                MeshCountMap[Mesh]++;
                if (RegisteredMeshes.find(Mesh) == RegisteredMeshes.end())
                {
                    RegisteredMeshes.emplace(Mesh);
                    NewMeshes.push_back(Mesh);
                }
                
                // This mesh needs a proxy created.
                if (MeshComponent.ProxyID == INDEX_NONE)
                {
                    FRenderProxy Proxy;
                    MeshComponent.ProxyID = (int64)RenderProxies.size();
                    Proxy.ProxyID = RenderProxies.size();
                    Proxy.Material = Mesh->GetMaterial();
                    Proxy.SortKey = ((uint64)Proxy.Material << 32) | ((uint64)Proxy.Mesh & 0xFFFFFFFF);
                    Proxy.Mesh = Mesh;
                    Proxy.Matrix = TransformComponent.GetTransform().GetMatrix();
                    RenderProxies.push_back(Proxy);
                    DirtyRenderProxies.push_back(Proxy.ProxyID);
                }
                else // Update current render proxy.
                {
                    FRenderProxy& Proxy = RenderProxies[MeshComponent.ProxyID];
                    Proxy.Material = Mesh->GetMaterial();
                    Proxy.Mesh = Mesh;
                    Proxy.SortKey = ((uint64)Proxy.Material << 32) | ((uint64)Proxy.Mesh & 0xFFFFFFFF);
                    Proxy.Matrix = TransformComponent.GetTransform().GetMatrix();
                    Proxy.ModelMatrixIndex = 0;
                    DirtyRenderProxies.push_back(Proxy.ProxyID);
                }
                (void)Entity.RemoveComponent<FNeedsRenderProxyUpdate>();
            }
        }

        SIZE_T CurrentOffset = NumMeshBlocks;
        SIZE_T VertexOffset = 0;
        SIZE_T IndexOffset = 0;

        THashMap<CStaticMesh*, SIZE_T> VertexStartMap;
        THashMap<CStaticMesh*, SIZE_T> IndexStartMap;

        uint32 VertexStreamSize = CombinedVertex.size() * sizeof(FVertex);
        uint32 IndexStreamSize = CombinedIndex.size() * sizeof(uint32);
        CreateOrResizeGeometryBuffers(VertexStreamSize, IndexStreamSize,
            VertexStreamSize + (NewMeshes.size() * 5000), IndexStreamSize + (NewMeshes.size() * 5000));
        
        for (CStaticMesh* NewMesh : NewMeshes)
        {
            const SIZE_T Count = MeshCountMap[NewMesh];
            MeshBlocks[NewMesh] = CurrentOffset;
            CurrentOffset += Count;

            const TVector<FVertex>& Vertices = NewMesh->GetMeshResource().Vertices;
            const TVector<uint32>& Indices = NewMesh->GetMeshResource().Indices;
            
            CombinedVertex.insert(CombinedVertex.end(), Vertices.begin(), Vertices.end());
            CombinedIndex.insert(CombinedIndex.end(), Indices.begin(), Indices.end());

            CommandList->WriteBuffer(VertexBuffer, CombinedVertex.data() + VertexOffset, 0, Vertices.size() * sizeof(FVertex));
            CommandList->WriteBuffer(IndexBuffer, CombinedIndex.data() + IndexOffset, 0, Indices.size() * sizeof(uint32));

            VertexStartMap.try_emplace(NewMesh, VertexOffset);
            IndexStartMap.try_emplace(NewMesh, IndexOffset);
            
            VertexOffset += Vertices.size();
            IndexOffset += Indices.size();
        }
        
        
        THashMap<CStaticMesh*, SIZE_T> MeshInstanceOffsets;
        for (const SIZE_T DirtyRenderProxy : DirtyRenderProxies)
        {
            FRenderProxy& Proxy = RenderProxies[DirtyRenderProxy];
            CStaticMesh* Mesh = Proxy.Mesh;

            SIZE_T BaseOffset = MeshBlocks[Mesh];
            SIZE_T& Offset = MeshInstanceOffsets[Mesh];

            Proxy.ModelMatrixIndex = BaseOffset + Offset;
            Offset++;

            Assert(Proxy.ModelMatrixIndex < MAX_MODELS);

            ModelData.ModelMatrix[Proxy.ModelMatrixIndex] = Proxy.Matrix;

            CommandList->WriteBuffer(ModelDataBuffer, &ModelData.ModelMatrix[Proxy.ModelMatrixIndex], Proxy.ModelMatrixIndex * sizeof(glm::mat4), sizeof(glm::mat4));
        }

        for (const SIZE_T DirtyRenderProxy : DirtyRenderProxies)
        {
            FRenderProxy& Proxy = RenderProxies[DirtyRenderProxy];
            uint64 Key = Proxy.SortKey;

            SIZE_T ArgumentIndex;
            if (IndirectArgsMap.find(Key) != IndirectArgsMap.end())
            {
                // Existing draw call → increment instance count
                ArgumentIndex = IndirectArgsMap[Key];
                DrawIndexedArguments[ArgumentIndex].InstanceCount += 1;
                CommandList->WriteBuffer(DrawCommandBuffer, &DrawIndexedArguments[ArgumentIndex], ArgumentIndex * sizeof(FDrawIndexedIndirectArguments), sizeof(FDrawIndexedIndirectArguments));
            }
            else
            {
                // New draw call → allocate and populate
                ArgumentIndex = DrawIndexedArguments.size();
                IndirectArgsMap[Key] = ArgumentIndex;

                FDrawIndexedIndirectArguments Cmd = {};
                Cmd.StartInstanceLocation   = Proxy.ModelMatrixIndex;
                Cmd.BaseVertexLocation      = VertexStartMap[Proxy.Mesh];
                Cmd.StartIndexLocation      = IndexStartMap[Proxy.Mesh];
                Cmd.IndexCount              = Proxy.Mesh->GetNumIndices();
                Cmd.InstanceCount           = 1;

                DrawIndexedArguments.push_back(Cmd);
                CommandList->WriteBuffer(DrawCommandBuffer, &DrawIndexedArguments[ArgumentIndex], ArgumentIndex * sizeof(FDrawIndexedIndirectArguments), sizeof(FDrawIndexedIndirectArguments));
            }
        }        
        
        uint32 DrawBufferSize = DrawList.DrawIndexedArguments.size() * sizeof(FDrawIndexedIndirectArguments);
        CreateOrResizeDrawCommandBuffer(DrawBufferSize, DrawBufferSize + 5000);

        CommandList->WriteBuffer(SceneDataBuffer, &SceneGlobalData, 0, sizeof(FSceneGlobalData));
        CommandList->WriteBuffer(LightDataBuffer, &DrawList.LightData, 0, sizeof(FSceneLightData));
        
        GeometryPass(DrawList);
        LightingPass(DrawList);
        SkyboxPass(DrawList);
        DrawPrimitives(DrawList);

        CommandList->SetImageState(GetRenderTarget(), AllSubresources, EResourceStates::ShaderResource);
        CommandList->CommitBarriers();

        LastFrameRenderData = Memory::Move(DrawList);
    }

    void FSceneRenderer::OnSwapchainResized()
    {
        CreateImages();
    }

    void FSceneRenderer::CreateOrResizeGeometryBuffers(uint64 VertexSize, uint64 IndexSize, uint64 VertexSizeDesired, uint64 IndexSizeDesired)
    {
        if (VertexSize != 0 && (VertexBuffer == nullptr || VertexBuffer->GetSize() < VertexSize))
        {
            FRHIBufferDesc VertexBufferDesc;
            VertexBufferDesc.Size = VertexSizeDesired;
            VertexBufferDesc.Stride = sizeof(FVertex);
            VertexBufferDesc.Usage.SetMultipleFlags(BUF_VertexBuffer);
            VertexBufferDesc.InitialState = EResourceStates::VertexBuffer;
            VertexBufferDesc.bKeepInitialState = true;
            VertexBufferDesc.DebugName = "Vertex Buffer";
            VertexBuffer = RenderContext->CreateBuffer(VertexBufferDesc);
            RenderContext->SetObjectName(VertexBuffer, "VertexBuffer", EAPIResourceType::Buffer);
        }

        if (IndexSize != 0 && (IndexBuffer == nullptr || IndexBuffer->GetSize() < IndexSize))
        {
            FRHIBufferDesc IndexBufferDesc;
            IndexBufferDesc.Size = IndexSizeDesired;
            IndexBufferDesc.Stride = sizeof(uint32);
            IndexBufferDesc.Usage.SetMultipleFlags(BUF_IndexBuffer);
            IndexBufferDesc.InitialState = EResourceStates::IndexBuffer;
            IndexBufferDesc.bKeepInitialState = true;
            IndexBufferDesc.DebugName = "Index Buffer";
            IndexBuffer = RenderContext->CreateBuffer(IndexBufferDesc);
            RenderContext->SetObjectName(IndexBuffer, "IndexBuffer", EAPIResourceType::Buffer);
        }
    }

    void FSceneRenderer::CreateOrResizeDrawCommandBuffer(uint64 SizeRequired, uint64 SizeDesired)
    {
        if (SizeDesired == 0)
        {
            return;
        }

        if (!DrawCommandBuffer || DrawCommandBuffer->GetSize() < SizeRequired)
        {
            FRHIBufferDesc BufferDesc;
            BufferDesc.Size = SizeDesired;
            BufferDesc.Stride = sizeof(FDrawIndexedIndirectArguments);
            BufferDesc.Usage.SetMultipleFlags(BUF_Indirect, BUF_Dynamic);
            BufferDesc.InitialState = EResourceStates::IndirectArgument;
            BufferDesc.bKeepInitialState = true;
            BufferDesc.MaxVersions = 3;
            BufferDesc.DebugName = "DrawCommandBuffer";

            DrawCommandBuffer = RenderContext->CreateBuffer(BufferDesc);
            RenderContext->SetObjectName(DrawCommandBuffer, BufferDesc.DebugName.c_str(), EAPIResourceType::Buffer);
        }
    }

    void FSceneRenderer::GeometryPass(const FSceneRenderData& DrawList)
    {
        LUMINA_PROFILE_SCOPE_COLORED(tracy::Color::Red);
        if (DrawList.DrawIndexedArguments.empty())
        {
            return;
        }
        
        ICommandList* CommandList = RenderContext->GetCommandList(Q_Graphics);
        
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
        for (SIZE_T CurrentDraw = 0; CurrentDraw < DrawList.RenderBatch.size(); ++CurrentDraw)
        {
            FIndirectRenderBatch Batch = DrawList.RenderBatch[CurrentDraw];
            CStaticMesh* Mesh = Batch.Mesh;
            CMaterial* Material = Batch.Material;

            FGraphicsPipelineDesc Desc;
            Desc.SetRenderState(RenderState);
            Desc.SetInputLayout(VertexLayoutInput);
            Desc.AddBindingLayout(BindingLayout);
            Desc.AddBindingLayout(Material->BindingLayout);
            Desc.SetVertexShader(Material->VertexShader);
            Desc.SetPixelShader(Material->PixelShader);
            
            FRHIGraphicsPipelineRef Pipeline = RenderContext->CreateGraphicsPipeline(Desc);
            
            CommandList->SetGraphicsPipeline(Pipeline);

            CommandList->BindBindingSets(ERHIBindingPoint::Graphics, {{BindingSet, 0}, {Material->BindingSet, 1}});
            
            CommandList->SetBufferState(DrawCommandBuffer, EResourceStates::IndirectArgument);
            CommandList->CommitBarriers();
            CommandList->DrawIndexedIndirect(DrawCommandBuffer, IndexBuffer, 1, CurrentDraw * sizeof(FDrawIndexedIndirectArguments));
            
            SceneRenderStats.NumVertices += Mesh->GetNumVertices() * Batch.Args.InstanceCount;
            SceneRenderStats.NumIndices += Mesh->GetNumIndices() * Batch.Args.InstanceCount;
            SceneRenderStats.NumDrawCalls++;
        }
        
        CommandList->EndRenderPass();
    }

    void FSceneRenderer::LightingPass(const FSceneRenderData& DrawList)
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
        
        CommandList->Draw(3, 1, 0, 0);

        CommandList->EndRenderPass();

    }

    void FSceneRenderer::SkyboxPass(const FSceneRenderData& DrawList)
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

        CommandList->Draw(36, 1, 0, 0);
        
        CommandList->EndRenderPass();
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

    void FSceneRenderer::DrawPrimitives(const FSceneRenderData& DrawList)
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
        
        for (uint32 CurrentDraw = 0; CurrentDraw < (uint32)DrawList.LightData.NumPointLights; ++CurrentDraw)
        {
            FPointLight Light = DrawList.LightData.PointLightsLights[CurrentDraw];

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
            CommandList->Draw(36, 1, 0, 0);
            
        }
        
        CommandList->EndRenderPass();
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
        BindingLayoutDesc.AddItem(FBindingLayoutItem(1, ERHIBindingResourceType::Buffer_Storage_Dynamic));
        BindingLayoutDesc.AddItem(FBindingLayoutItem(2, ERHIBindingResourceType::Buffer_Storage_Dynamic));
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
        SceneDataBuffer = RenderContext->CreateBuffer(BufferDesc);
        RenderContext->SetObjectName(SceneDataBuffer, "SceneGlobalData", EAPIResourceType::Buffer);

        FRHIBufferDesc ModelBufferDesc;
        ModelBufferDesc.Size = sizeof(FModelData);
        ModelBufferDesc.Stride = sizeof(FModelData);
        ModelBufferDesc.Usage.SetMultipleFlags(BUF_StorageBuffer, BUF_Dynamic);
        ModelBufferDesc.MaxVersions = 3;
        ModelDataBuffer = RenderContext->CreateBuffer(ModelBufferDesc);
        RenderContext->SetObjectName(ModelDataBuffer, "ModelDataBuffer", EAPIResourceType::Buffer);
        
        FRHIBufferDesc LightBufferDesc;
        LightBufferDesc.Size = sizeof(FSceneLightData);
        LightBufferDesc.Stride = sizeof(FSceneLightData);
        LightBufferDesc.Usage.SetMultipleFlags(BUF_StorageBuffer, BUF_Dynamic);
        LightBufferDesc.MaxVersions = 3;
        LightDataBuffer = RenderContext->CreateBuffer(LightBufferDesc);
        RenderContext->SetObjectName(LightDataBuffer, "LightDataBuffer", EAPIResourceType::Buffer);
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
            std::filesystem::path ResourceDirectory = Paths::GetEngineResourceDirectory();
            ResourceDirectory /= std::filesystem::path("Textures") / "CubeMaps" / "Mountains" / CubeFaceFiles[i];
            
            TVector<uint8> Pixels;
            FIntVector2D Extent = ImportHelpers::GetImagePixelData(Pixels, ResourceDirectory.generic_string().c_str(), false);
            
            const uint32 width = Extent.X;
            const uint32 height = Extent.Y;
            const SIZE_T rowPitch = width * 4;  // 4 bytes per pixel (RGBA8)
            const SIZE_T depthPitch = rowPitch * height;
            
            CommandList->WriteImage(CubeMap, i, 0, Pixels.data(), rowPitch, depthPitch);
        }

        CommandList->SetPermanentImageState(CubeMap, EResourceStates::ShaderResource);
        CommandList->CommitBarriers();
        
        CommandList->Close();
    	RenderContext->ExecuteCommandList(CommandList, ECommandQueue::Transfer);
    }
    
}
