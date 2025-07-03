#include "SceneRenderer.h"

#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Core/Windows/Window.h"
#include "Entity/Components/CameraComponent.h"
#include "Entity/Entity.h"
#include "Renderer/RHIIncl.h"
#include "Scene.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include "Core/Engine/Engine.h"
#include "Core/Profiler/Profile.h"
#include "Entity/Components/StaicMeshComponent.h"
#include "Paths/Paths.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"
#include "Subsystems/FCameraManager.h"
#include "Tools/Import/ImportHelpers.h"

namespace Lumina
{
    FSceneRenderer::FSceneRenderer()
        : SceneGlobalData()
    {
        SceneViewport = GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext()->CreateViewport(Windowing::GetPrimaryWindowHandle()->GetExtent());
    }

    FSceneRenderer::~FSceneRenderer()
    {
    }

    void FSceneRenderer::Initialize()
    {
        InitBuffers();
        CreateImages();
    }

    void FSceneRenderer::Deinitialize()
    {
        
    }

    void FSceneRenderer::StartScene(const FScene* Scene)
    {
        LUMINA_PROFILE_SCOPE();
        
        IRenderContext* RenderContext = GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext();

        ICommandList* CommandList = RenderContext->GetCommandList();

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
        //FullScreenPass(Scene);
        DrawSkybox(Scene);
        ForwardRenderPass(Scene);
    }

    void FSceneRenderer::OnSwapchainResized()
    {
        CreateImages();
    }

    void FSceneRenderer::DrawSkybox(const FScene* Scene)
    {
        IRenderContext* RenderContext = GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext();
        ICommandList* CommandList = RenderContext->GetCommandList();

        FRHIVertexShaderRef VertexShader = RenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("Skybox.vert");
        FRHIPixelShaderRef PixelShader = RenderContext->GetShaderLibrary()->GetShader<FRHIPixelShader>("Skybox.frag");
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
        DepthState.SetDepthTestEnable(false);
        DepthState.SetDepthWriteEnable(false);
        
        FBlendState BlendState;
        FBlendState::RenderTarget RenderTarget;
        RenderTarget.EnableBlend();
        RenderTarget.SetSrcBlend(EBlendFactor::SrcAlpha);
        RenderTarget.SetDestBlend(EBlendFactor::OneMinusSrcAlpha);
        RenderTarget.SetSrcBlendAlpha(EBlendFactor::One);
        RenderTarget.SetDestBlendAlpha(EBlendFactor::Zero);
        BlendState.SetRenderTarget(0, RenderTarget);

        FRasterState RasterState;
        RasterState.SetCullNone();
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
        .AddColorAttachment(GetRenderTarget())
        .SetColorLoadOp(ERenderLoadOp::Load)
        .SetColorStoreOp(ERenderStoreOp::Store)
        .SetColorClearColor(FColor::Black)
                
        .SetDepthAttachment(DepthBuffer)
        .SetDepthClearValue(0.0f)
        .SetDepthLoadOp(ERenderLoadOp::Clear)
        .SetDepthStoreOp(ERenderStoreOp::Store)
                
        .SetRenderArea(GetRenderTarget()->GetExtent());
        CommandList->BeginRenderPass(BeginInfo);

        CommandList->SetViewport(0.0f, 0.0f, 0.0f, (float)GetRenderTarget()->GetSizeX(), (float)GetRenderTarget()->GetSizeY(), 1.0f);
        CommandList->SetScissorRect(0, 0, GetRenderTarget()->GetSizeX(), GetRenderTarget()->GetSizeY());

        CommandList->Draw(36, 1, 0, 0);
        
        CommandList->EndRenderPass();

    }

    void FSceneRenderer::ForwardRenderPass(const FScene* Scene)
    {
        IRenderContext* RenderContext = GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext();
        ICommandList* CommandList = RenderContext->GetCommandList();

        FRHIVertexShaderRef VertexShader = RenderContext->GetShaderLibrary()->GetShader<FRHIVertexShader>("PrimitiveMesh.vert");
        FRHIPixelShaderRef PixelShader = RenderContext->GetShaderLibrary()->GetShader<FRHIPixelShader>("PrimitiveMesh.frag");
        if (!VertexShader || !PixelShader)
        {
            return;
        }
        
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
        LayoutDesc.StageFlags.SetFlag(ERHIShaderType::Vertex);
        FRHIBindingLayoutRef Layout = RenderContext->CreateBindingLayout(LayoutDesc);

        FBindingSetDesc SetDesc;
        SetDesc.AddItem(FBindingSetItem::BufferCBV(0, SceneDataBuffer));
        SetDesc.AddItem(FBindingSetItem::BufferUAV(1, ModelDataBuffer));
        FRHIBindingSetRef Set = RenderContext->CreateBindingSet(SetDesc, Layout);

        FDepthStencilState DepthState;
        DepthState.SetDepthFunc(EComparisonFunc::Greater);
        
        FBlendState BlendState;
        FBlendState::RenderTarget RenderTarget;
        RenderTarget.EnableBlend();
        RenderTarget.SetSrcBlend(EBlendFactor::SrcAlpha);
        RenderTarget.SetDestBlend(EBlendFactor::OneMinusSrcAlpha);
        RenderTarget.SetSrcBlendAlpha(EBlendFactor::One);
        RenderTarget.SetDestBlendAlpha(EBlendFactor::Zero);
        BlendState.SetRenderTarget(0, RenderTarget);

        FRasterState RasterState;
        RasterState.SetDepthClipEnable(true);
        
        FRenderState RenderState;
        RenderState.SetBlendState(BlendState);
        RenderState.SetRasterState(RasterState);
        RenderState.SetDepthStencilState(DepthState);
        
        FGraphicsPipelineDesc Desc;
        Desc.SetRenderState(RenderState);
        Desc.SetInputLayout(VertexLayoutInput);
        Desc.AddBindingLayout(Layout);
        Desc.SetVertexShader(VertexShader);
        Desc.SetPixelShader(PixelShader);

        FRHIGraphicsPipelineRef Pipeline = RenderContext->CreateGraphicsPipeline(Desc);

        CommandList->SetGraphicsPipeline(Pipeline);

        CommandList->BindBindingSets({Set}, ERHIBindingPoint::Graphics);
        
        for (auto& Entity : Scene->GetConstEntityRegistry().view<FStaticMeshComponent>())
        {
            const FStaticMeshComponent& MeshComponent = Scene->GetConstEntityRegistry().get<FStaticMeshComponent>(Entity);
            
            CStaticMesh* Mesh = MeshComponent.StaticMesh;
            if (Mesh == nullptr)
            {
                continue;
            }
                
            const FTransformComponent& TransformComponent = Scene->GetConstEntityRegistry().get<FTransformComponent>(Entity);

            glm::mat4 ModelMatrix = glm::mat4(1.0f);
            ModelMatrix = glm::translate(ModelMatrix, TransformComponent.GetLocation());

            glm::quat Rotation = TransformComponent.GetRotation();
            ModelMatrix *= glm::toMat4(Rotation);

            ModelMatrix = glm::scale(ModelMatrix, TransformComponent.GetScale());
            ModelData.push_back({ModelMatrix});
        }

        if (!ModelData.empty())
        {
            CommandList->UploadToBuffer(ModelDataBuffer, ModelData.data(), 0, sizeof(FModelData) * (uint32)ModelData.size());
        }
        
        FRenderPassBeginInfo BeginInfo; BeginInfo
        .AddColorAttachment(GetRenderTarget())
        .SetColorLoadOp(ERenderLoadOp::Load)
        .SetColorStoreOp(ERenderStoreOp::Store)
        .SetColorClearColor(FColor::Black)
        
        .SetDepthAttachment(DepthBuffer)
        .SetDepthClearValue(0.0f)
        .SetDepthLoadOp(ERenderLoadOp::Load)
        .SetDepthStoreOp(ERenderStoreOp::Store)
        
        .SetRenderArea(GetRenderTarget()->GetExtent());
        CommandList->BeginRenderPass(BeginInfo);

        CommandList->SetViewport(0.0f, 0.0f, 0.0f, (float)GetRenderTarget()->GetSizeX(), (float)GetRenderTarget()->GetSizeY(), 1.0f);
        CommandList->SetScissorRect(0, 0, GetRenderTarget()->GetSizeX(), GetRenderTarget()->GetSizeY());

        uint32 Index = 0;
        for (auto& Entity : Scene->GetConstEntityRegistry().view<FStaticMeshComponent>())
        {
            const FStaticMeshComponent& MeshComponent = Scene->GetConstEntityRegistry().get<FStaticMeshComponent>(Entity);
            
            CStaticMesh* Mesh = MeshComponent.StaticMesh;
            if (Mesh == nullptr)
            {
                continue;
            }
                
            FRHIBufferRef MeshVertexBuffer = Mesh->GetVertexBuffer();
            FRHIBufferRef MeshIndexBuffer = Mesh->GetIndexBuffer();
            

            CommandList->SetPushConstants(&Index, sizeof(uint32));
            CommandList->BindVertexBuffer(MeshVertexBuffer, 0, 0);
            CommandList->DrawIndexed(MeshIndexBuffer, Mesh->GetNumIndicies(), 1, 0);
            
            Index++;
        }
        
        CommandList->EndRenderPass();
    }

    void FSceneRenderer::FullScreenPass(const FScene* Scene)
    {
        IRenderContext* RenderContext = GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext();
        ICommandList* CommandList = RenderContext->GetCommandList();
        
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
    }

    void FSceneRenderer::DrawPrimitives(const FScene* Scene)
    {
        
    }
    
    void FSceneRenderer::InitBuffers()
    {
        IRenderContext* RenderContext = GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext();

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
    }
    
    void FSceneRenderer::CreateImages()
    {
        IRenderContext* RenderContext = GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext();
        
        FRHIImageDesc DepthImageDesc;
        DepthImageDesc.Extent = Windowing::GetPrimaryWindowHandle()->GetExtent();
        DepthImageDesc.Flags.SetFlag(EImageCreateFlags::DepthStencil);
        DepthImageDesc.Format = EImageFormat::D32;
        DepthImageDesc.Dimension = EImageDimension::Texture2D;

        DepthBuffer = RenderContext->CreateImage(DepthImageDesc);


        FRHIImageDesc SkyCubeMapDesc;
        SkyCubeMapDesc.Extent = {2048, 2048};
        SkyCubeMapDesc.Flags.SetFlag(EImageCreateFlags::CubeCompatible);
        SkyCubeMapDesc.Flags.SetFlag(EImageCreateFlags::ShaderResource);
        SkyCubeMapDesc.Format = EImageFormat::RGBA32_UNORM;
        SkyCubeMapDesc.Dimension = EImageDimension::Texture2D;
        SkyCubeMapDesc.ArraySize = 6;

        CubeMap = RenderContext->CreateImage(SkyCubeMapDesc);
        RenderContext->SetObjectName(CubeMap, "CubeMap", EAPIResourceType::Image);

        static const char* CubeFaceFiles[6] = {
            "right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg"
        };

        ICommandList* CommandList = RenderContext->CreateCommandList();
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
    	RenderContext->ExecuteCommandList(CommandList);
    }
    
}
