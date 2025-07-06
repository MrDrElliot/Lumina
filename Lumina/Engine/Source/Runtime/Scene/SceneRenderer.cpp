#include "SceneRenderer.h"

#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Core/Windows/Window.h"
#include "Entity/Components/CameraComponent.h"
#include "Entity/Entity.h"
#include "Renderer/RHIIncl.h"
#include "Scene.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "Assets/AssetTypes/Textures/Texture.h"
#include "glm/gtx/quaternion.hpp"
#include "Core/Engine/Engine.h"
#include "Core/Profiler/Profile.h"
#include "Entity/Components/StaicMeshComponent.h"
#include "Paths/Paths.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"
#include "Rendering/ClearColorRenderPass.h"
#include "Rendering/RenderGraphResources.h"
#include "Rendering/SkyboxRenderPass.h"
#include "Subsystems/FCameraManager.h"
#include "Tools/Import/ImportHelpers.h"

namespace Lumina
{
    
    FSceneRenderer::FSceneRenderer()
        : SceneGlobalData()
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
        
        InitBuffers();
        CreateImages();
        InitResources();
        

        RenderGraph.AddResource<PrimaryRenderTargetTag>(GetRenderTarget());
        RenderGraph.AddResource<DepthBufferTargetTag>(DepthAttachment);
        RenderGraph.AddResource<SceneGlobalDataTag>(SceneDataBuffer);
        RenderGraph.AddResource<CubeMapImageTag>(CubeMap);
        
        RenderGraph.AddRenderPass<FClearColorRenderPass>("ClearColorPass", [] (FRenderGraphBuilder& Builder)
        {
            Builder.Write<PrimaryRenderTargetTag>(ERHIAccess::Write);
        });
        
        //RenderGraph.AddRenderPass<FSkyboxRenderPass>("SkyBox", [] (FRenderGraphBuilder& Builder)
        //{
        //    Builder.Write<PrimaryRenderTargetTag>(ERHIAccess::ColorAttachmentWrite);
        //    Builder.Write<DepthBufferTargetTag>(ERHIAccess::DepthStencilAttachmentWrite);
        //    Builder.Read<SceneGlobalDataTag>(ERHIAccess::Read);
        //    Builder.Read<CubeMapImageTag>(ERHIAccess::ShaderRead);
        //});
    }

    void FSceneRenderer::Deinitialize()
    {
        RenderGraph.ClearPasses();
    }

    void FSceneRenderer::StartScene(const FScene* Scene)
    {
        LUMINA_PROFILE_SCOPE();
        
        ICommandList* CommandList = RenderContext->GetCommandList(Q_Graphics);

        FCameraManager* CameraManager = Scene->GetSceneSubsystem<FCameraManager>();
        FCameraComponent& CameraComponent = CameraManager->GetActiveCameraEntity().GetComponent<FCameraComponent>();

        SceneGlobalData.CameraData.Location =   glm::vec4(CameraComponent.GetPosition(), 1.0f);
        SceneGlobalData.CameraData.View =       CameraComponent.GetViewMatrix();
        SceneGlobalData.CameraData.Projection = CameraComponent.GetProjectionMatrix();
        SceneGlobalData.Time =                  (float)Scene->GetTimeSinceSceneCreation();
        SceneGlobalData.DeltaTime =             (float)Scene->GetSceneDeltaTime(); 
        
        SceneViewport->SetViewVolume(CameraComponent.GetViewVolume());
        
        CommandList->UploadToBuffer(SceneDataBuffer, &SceneGlobalData, 0, sizeof(FSceneGlobalData));
        
        ModelData.clear();
    }

    void FSceneRenderer::EndScene(const FScene* Scene)
    {
        ICommandList* CommandList = RenderContext->GetCommandList(Q_Graphics);

        FFrameDrawList DrawList;
        DrawList.Meshes.reserve(100);
        DrawList.ModelData.reserve(100);

        for (auto& Entity : Scene->GetConstEntityRegistry().view<FStaticMeshComponent>())
        {
            const FStaticMeshComponent& MeshComponent = Scene->GetConstEntityRegistry().get<FStaticMeshComponent>(Entity);
            const FTransformComponent& TransformComponent = Scene->GetConstEntityRegistry().get<FTransformComponent>(Entity);
            CStaticMesh* Mesh = MeshComponent.StaticMesh;
            if (!Mesh->IsReadyForRender())
            {
                continue;
            }

            glm::mat4 ModelMatrix = glm::mat4(1.0f);
            ModelMatrix = glm::translate(ModelMatrix, TransformComponent.GetLocation());

            glm::quat Rotation = TransformComponent.GetRotation();
            ModelMatrix *= glm::toMat4(Rotation);
            ModelMatrix = glm::scale(ModelMatrix, TransformComponent.GetScale());
            
            DrawList.ModelData.push_back({ModelMatrix});
            DrawList.Meshes.push_back(Mesh);
        }

        if (!DrawList.ModelData.empty())
        {
            CommandList->UploadToBuffer(ModelDataBuffer, DrawList.ModelData.data(), 0, sizeof(FModelData) * (uint32)DrawList.ModelData.size());
        }

        
        RenderGraph.Compile();
        RenderGraph.Execute(CommandList);
        GeometryPass(DrawList);
        LightingPass(DrawList);
        SkyboxPass(DrawList);

        CommandList->SetRequiredImageAccess(GetRenderTarget(), ERHIAccess::ShaderRead);
        CommandList->CommitBarriers();
    }

    void FSceneRenderer::OnSwapchainResized()
    {
        CreateImages();
    }
    

    void FSceneRenderer::GeometryPass(const FFrameDrawList& DrawList)
    {
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

        .AddColorAttachment(GBuffer.AlbedoSpec)
        .SetColorLoadOp(ERenderLoadOp::Clear)
        .SetColorStoreOp(ERenderStoreOp::Store)
        .SetColorClearColor(FColor::Black)

        .SetDepthAttachment(DepthAttachment)
        .SetDepthClearValue(0.0f)
        .SetDepthLoadOp(ERenderLoadOp::Clear)
        .SetDepthStoreOp(ERenderStoreOp::Store)        
            
        .SetRenderArea(GetRenderTarget()->GetExtent());
        CommandList->BeginRenderPass(BeginInfo);

        
        for (uint32 CurrentDraw = 0; CurrentDraw < (uint32)DrawList.Meshes.size(); ++CurrentDraw)
        {
            CStaticMesh* Mesh = DrawList.Meshes[CurrentDraw];
            const FModelData& ModelData = DrawList.ModelData[CurrentDraw];

            FDepthStencilState DepthState;
            DepthState.SetDepthFunc(EComparisonFunc::Greater);
        
            FBlendState BlendState;
            FBlendState::RenderTarget AlbedoTarget;
            AlbedoTarget.DisableBlend();
            AlbedoTarget.SetSrcBlend(EBlendFactor::SrcAlpha);
            AlbedoTarget.SetDestBlend(EBlendFactor::OneMinusSrcAlpha);
            AlbedoTarget.SetSrcBlendAlpha(EBlendFactor::One);
            AlbedoTarget.SetDestBlendAlpha(EBlendFactor::Zero);
            AlbedoTarget.SetFormat(EFormat::RGBA8_UNORM);
            BlendState.SetRenderTarget(0, AlbedoTarget);

            FBlendState::RenderTarget NormalTarget;
            NormalTarget.DisableBlend();
            NormalTarget.SetSrcBlend(EBlendFactor::SrcAlpha);
            NormalTarget.SetDestBlend(EBlendFactor::OneMinusSrcAlpha);
            NormalTarget.SetSrcBlendAlpha(EBlendFactor::One);
            NormalTarget.SetDestBlendAlpha(EBlendFactor::Zero);
            NormalTarget.SetFormat(EFormat::RG16_SNORM);
            BlendState.SetRenderTarget(1, NormalTarget);

            FBlendState::RenderTarget MaterialTarget;
            MaterialTarget.DisableBlend();
            MaterialTarget.SetSrcBlend(EBlendFactor::SrcAlpha);
            MaterialTarget.SetDestBlend(EBlendFactor::OneMinusSrcAlpha);
            MaterialTarget.SetSrcBlendAlpha(EBlendFactor::One);
            MaterialTarget.SetDestBlendAlpha(EBlendFactor::Zero);
            MaterialTarget.SetFormat(EFormat::RGBA8_UNORM);
            BlendState.SetRenderTarget(2, MaterialTarget);

            FRasterState RasterState;
            RasterState.SetDepthClipEnable(true);
            
            FRenderState RenderState;
            RenderState.SetBlendState(BlendState);
            RenderState.SetRasterState(RasterState);
            RenderState.SetDepthStencilState(DepthState);
            
            FGraphicsPipelineDesc Desc;
            Desc.SetRenderState(RenderState);
            Desc.SetInputLayout(VertexLayoutInput);
            Desc.AddBindingLayout(SceneGlobalBindingLayout);
            Desc.AddBindingLayout(Mesh->GetMaterial()->BindingLayout);
            Desc.SetVertexShader(Mesh->GetMaterial()->VertexShader);
            Desc.SetPixelShader(Mesh->GetMaterial()->PixelShader);
            
            FRHIGraphicsPipelineRef Pipeline = RenderContext->CreateGraphicsPipeline(Desc);
            
            CommandList->SetGraphicsPipeline(Pipeline);
            
            CommandList->BindBindingSets({SceneGlobalBindingSet, Mesh->GetMaterial()->BindingSet}, ERHIBindingPoint::Graphics);
            
            float SizeY = (float)GBuffer.AlbedoSpec->GetSizeY();
            float SizeX = (float)GBuffer.AlbedoSpec->GetSizeX();
            CommandList->SetViewport(0.0f, 0.0f, 0.0f, SizeX, SizeY, 1.0f);
            CommandList->SetScissorRect(0, 0, SizeX, SizeY);
        
            
            FRHIBufferRef MeshVertexBuffer = Mesh->GetVertexBuffer();
            FRHIBufferRef MeshIndexBuffer = Mesh->GetIndexBuffer();
            

            CommandList->SetPushConstants(&CurrentDraw, sizeof(uint32));
            CommandList->BindVertexBuffer(MeshVertexBuffer, 0, 0);
            CommandList->DrawIndexed(MeshIndexBuffer, Mesh->GetNumIndicies(), 1, 0);
            
        }
        
        CommandList->EndRenderPass();
    }

    void FSceneRenderer::LightingPass(const FFrameDrawList& DrawList)
    {
        FRHIVertexShaderRef VertexShader = RenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("DeferredLighting.vert");
        FRHIPixelShaderRef PixelShader = RenderContext->GetShaderLibrary()->GetShader<FRHIPixelShader>("DeferredLighting.frag");
        if (!VertexShader || !PixelShader)
        {
            return;
        }
        
        ICommandList* CommandList = RenderContext->GetCommandList(Q_Graphics);
        
        FBindingLayoutItem Item_Albedo;
        Item_Albedo.Slot = 0;
        Item_Albedo.Type = ERHIBindingResourceType::Texture_SRV;

        FBindingLayoutItem Item_Normal;
        Item_Normal.Slot = 1;
        Item_Normal.Type = ERHIBindingResourceType::Texture_SRV;

        FBindingLayoutItem Item_Material;
        Item_Material.Slot = 2;
        Item_Material.Type = ERHIBindingResourceType::Texture_SRV;

        FBindingLayoutItem Item_Depth;
        Item_Depth.Slot = 3;
        Item_Depth.Type = ERHIBindingResourceType::Texture_SRV;
        
        FBindingLayoutDesc LayoutDesc;
        LayoutDesc.AddItem(Item_Albedo);
        LayoutDesc.AddItem(Item_Normal);
        LayoutDesc.AddItem(Item_Material);
        LayoutDesc.AddItem(Item_Depth);
        LayoutDesc.StageFlags.SetMultipleFlags(ERHIShaderType::Fragment);
        FRHIBindingLayoutRef LightingPassLayout = RenderContext->CreateBindingLayout(LayoutDesc);

        FBindingSetDesc SetDesc;
        SetDesc.AddItem(FBindingSetItem::TextureSRV(0, GBuffer.Position));
        SetDesc.AddItem(FBindingSetItem::TextureSRV(1, GBuffer.Normals));
        SetDesc.AddItem(FBindingSetItem::TextureSRV(2, GBuffer.AlbedoSpec));
        FRHIBindingSetRef LightingPassSet = RenderContext->CreateBindingSet(SetDesc, LightingPassLayout);
        
        FRasterState RasterState;
        RasterState.SetCullNone();

        FBlendState BlendState;
        FBlendState::RenderTarget RenderTarget;
        RenderTarget.DisableBlend();
        RenderTarget.SetSrcBlend(EBlendFactor::SrcAlpha);
        RenderTarget.SetDestBlend(EBlendFactor::OneMinusSrcAlpha);
        RenderTarget.SetSrcBlendAlpha(EBlendFactor::Zero);
        RenderTarget.SetDestBlendAlpha(EBlendFactor::One);
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
        Desc.AddBindingLayout(LightingPassLayout);
        Desc.SetVertexShader(VertexShader);
        Desc.SetPixelShader(PixelShader);

        FRHIGraphicsPipelineRef Pipeline = RenderContext->CreateGraphicsPipeline(Desc);
            
        CommandList->SetGraphicsPipeline(Pipeline);
        CommandList->BindBindingSets({LightingPassSet}, ERHIBindingPoint::Graphics);
        
        FRenderPassBeginInfo BeginInfo; BeginInfo
        .AddColorAttachment(GetRenderTarget())
        .SetColorLoadOp(ERenderLoadOp::Load)
        .SetColorStoreOp(ERenderStoreOp::Store)
        .SetColorClearColor(FColor::Black)

        .SetDepthAttachment(DepthAttachment)
        .SetDepthLoadOp(ERenderLoadOp::Load)
        .SetDepthStoreOp(ERenderStoreOp::Store)
        
        .SetRenderArea(GetRenderTarget()->GetExtent());
        CommandList->BeginRenderPass(BeginInfo);
        
            
        float SizeY = (float)GBuffer.AlbedoSpec->GetSizeY();
        float SizeX = (float)GBuffer.AlbedoSpec->GetSizeX();
        CommandList->SetViewport(0.0f, 0.0f, 0.0f, SizeX, SizeY, 1.0f);
        CommandList->SetScissorRect(0, 0, SizeX, SizeY);
        
        CommandList->Draw(3, 1, 0, 0);

        CommandList->EndRenderPass();

    }

    void FSceneRenderer::SkyboxPass(const FFrameDrawList& DrawList)
    {
        ICommandList* CommandList = RenderContext->GetCommandList(ECommandQueue::Graphics);

        FRHIVertexShaderRef VertexShader = RenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("Skybox.vert");
        FRHIPixelShaderRef PixelShader = RenderContext->GetShaderLibrary()->GetShader<FRHIPixelShader>("Skybox.frag");
        if (!VertexShader || !PixelShader)
        {
            return;
        }

        FRHIImageRef ColorAttachment = GetRenderTarget();
        
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
        
        FBindingLayoutItem ImageItem;
        ImageItem.Slot = 0;
        ImageItem.Type = ERHIBindingResourceType::Texture_SRV;

        FBindingLayoutDesc ImageLayoutDesc;
        ImageLayoutDesc.AddItem(ImageItem);
        ImageLayoutDesc.StageFlags.SetFlag(ERHIShaderType::Fragment);
        FRHIBindingLayoutRef Layout2 = RenderContext->CreateBindingLayout(ImageLayoutDesc);

        FBindingSetDesc SetDesc2;
        SetDesc2.AddItem(FBindingSetItem::TextureSRV(0, CubeMap));
        FRHIBindingSetRef Set2 = RenderContext->CreateBindingSet(SetDesc2, Layout2);
        
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
        Desc.AddBindingLayout(Layout);
        Desc.AddBindingLayout(Layout2);
        Desc.SetVertexShader(VertexShader);
        Desc.SetPixelShader(PixelShader);
  
        FRHIGraphicsPipelineRef Pipeline = RenderContext->CreateGraphicsPipeline(Desc);
        
        CommandList->SetGraphicsPipeline(Pipeline);

        CommandList->BindBindingSets({Set, Set2}, ERHIBindingPoint::Graphics);

        FRenderPassBeginInfo BeginInfo; BeginInfo
        .AddColorAttachment(ColorAttachment)
        .SetColorLoadOp(ERenderLoadOp::Load)
        .SetColorStoreOp(ERenderStoreOp::Store)
        .SetColorClearColor(FColor::Black)
                
        .SetDepthAttachment(DepthAttachment)
        .SetDepthLoadOp(ERenderLoadOp::Load)
        .SetDepthStoreOp(ERenderStoreOp::DontCare)
                
        .SetRenderArea(ColorAttachment->GetExtent());
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

    void FSceneRenderer::DrawPrimitives(const FScene* Scene)
    {
        
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

        FBindingLayoutItem Item;
        Item.Size = sizeof(FSceneGlobalData);
        Item.Slot = 0;
        Item.Type = ERHIBindingResourceType::Buffer_CBV;

        FBindingLayoutItem ModelItem;
        ModelItem.Size = sizeof(FModelData) * 1000;
        ModelItem.Slot = 1;
        ModelItem.Type = ERHIBindingResourceType::Buffer_UAV;

        FBindingLayoutItem PushConstants;
        PushConstants.Size = sizeof(uint32);
        PushConstants.Slot = 0;
        PushConstants.Type = ERHIBindingResourceType::PushConstants;
        
        FBindingLayoutDesc LayoutDesc;
        LayoutDesc.AddItem(Item);
        LayoutDesc.AddItem(ModelItem);
        LayoutDesc.AddItem(PushConstants);
        LayoutDesc.StageFlags.SetMultipleFlags(ERHIShaderType::Vertex, ERHIShaderType::Fragment);
        SceneGlobalBindingLayout = RenderContext->CreateBindingLayout(LayoutDesc);

        FBindingSetDesc SetDesc;
        SetDesc.AddItem(FBindingSetItem::BufferCBV(0, SceneDataBuffer));
        SetDesc.AddItem(FBindingSetItem::BufferUAV(1, ModelDataBuffer));
        SceneGlobalBindingSet = RenderContext->CreateBindingSet(SetDesc, SceneGlobalBindingLayout);
        
    }

    void FSceneRenderer::InitBuffers()
    {
        FRHIBufferDesc BufferDesc;
        BufferDesc.Size = sizeof(FSceneGlobalData);
        BufferDesc.Stride = sizeof(FSceneGlobalData);
        BufferDesc.Usage.SetFlag(BUF_UniformBuffer);
        SceneDataBuffer = RenderContext->CreateBuffer(BufferDesc);
        RenderContext->SetObjectName(SceneDataBuffer, "SceneGlobalData", EAPIResourceType::Buffer);

        FRHIBufferDesc ModelBufferDesc;
        ModelBufferDesc.Size = sizeof(FModelData) * 1000;
        ModelBufferDesc.Stride = sizeof(FModelData);
        ModelBufferDesc.Usage.SetFlag(BUF_StorageBuffer);
        ModelDataBuffer = RenderContext->CreateBuffer(ModelBufferDesc);
        RenderContext->SetObjectName(ModelDataBuffer, "ModelDataBuffer", EAPIResourceType::Buffer);
        
    }
    
    void FSceneRenderer::CreateImages()
    {
        FIntVector2D Extent = Windowing::GetPrimaryWindowHandle()->GetExtent();
        
        FRHIImageDesc GBufferPosition;
        GBufferPosition.Extent = Extent;
        GBufferPosition.Flags.SetMultipleFlags(EImageCreateFlags::ColorAttachment, EImageCreateFlags::ShaderResource);
        GBufferPosition.Format = EFormat::RGBA8_UNORM;
        GBufferPosition.Dimension = EImageDimension::Texture2D;
        
        GBuffer.Position = RenderContext->CreateImage(GBufferPosition);
        RenderContext->SetObjectName(GBuffer.Position, "GBuffer - Position", EAPIResourceType::Image);

        FRHIImageDesc NormalDesc = {};
        NormalDesc.Extent = Extent;
        NormalDesc.Format = EFormat::RG16_SNORM;
        NormalDesc.Dimension = EImageDimension::Texture2D;
        NormalDesc.Flags.SetMultipleFlags(EImageCreateFlags::ColorAttachment, EImageCreateFlags::ShaderResource);
        
        GBuffer.Normals = RenderContext->CreateImage(NormalDesc);
        RenderContext->SetObjectName(GBuffer.Normals, "GBuffer - Normals", EAPIResourceType::Image);

        FRHIImageDesc AlbedoDesc = {};
        AlbedoDesc.Extent = Extent;
        AlbedoDesc.Format = EFormat::RGBA8_UNORM;
        AlbedoDesc.Dimension = EImageDimension::Texture2D;
        AlbedoDesc.Flags.SetMultipleFlags(EImageCreateFlags::ColorAttachment, EImageCreateFlags::ShaderResource);
        
        GBuffer.AlbedoSpec = RenderContext->CreateImage(AlbedoDesc);
        RenderContext->SetObjectName(GBuffer.AlbedoSpec, "GBuffer - Albedo", EAPIResourceType::Image);
        
        
        FRHIImageDesc DepthImageDesc;
        DepthImageDesc.Extent = Extent;
        DepthImageDesc.Flags.SetMultipleFlags(EImageCreateFlags::DepthAttachment, EImageCreateFlags::ShaderResource);
        DepthImageDesc.Format = EFormat::D32;
        DepthImageDesc.Dimension = EImageDimension::Texture2D;

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

        CubeMap = RenderContext->CreateImage(SkyCubeMapDesc);
        RenderContext->SetObjectName(CubeMap, "CubeMap", EAPIResourceType::Image);

        static const char* CubeFaceFiles[6] = {
            "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg"
        };

        ICommandList* CommandList = RenderContext->CreateCommandList({Q_Transfer});
        CommandList->Open();
        
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
            
            CommandList->WriteToImage(CubeMap, i, 0, Pixels.data(), rowPitch, depthPitch);
        }
        
        CommandList->Close();
    	RenderContext->ExecuteCommandList(CommandList, 1, Q_Transfer);
    }
    
}
