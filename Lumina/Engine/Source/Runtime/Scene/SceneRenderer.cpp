#include "SceneRenderer.h"

#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Assets/AssetTypes/Mesh/StaticMesh/StaticMesh.h"
#include "Assets/Factories/MeshFactory/StaticMeshFactory.h"
#include "Core/Performance/PerformanceTracker.h"
#include "Core/Windows/Window.h"
#include "Entity/Components/CameraComponent.h"
#include "Entity/Entity.h"
#include "glm/gtc/type_ptr.hpp"
#include "Renderer/RHIIncl.h"
#include "Scene.h"
#include "Core/Engine/Engine.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderManager.h"
#include "Subsystems/FCameraManager.h"

namespace Lumina
{
    FSceneRenderer* FSceneRenderer::Create()
    {
        return Memory::New<FSceneRenderer>();
    }

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
        CreateImages();
    }

    void FSceneRenderer::Deinitialize()
    {
        
    }

    void FSceneRenderer::StartScene(const FScene* Scene)
    {
        IRenderContext* RenderContext = GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext();
        ICommandList* CommandList = RenderContext->GetCommandList();

        FCameraManager* CameraManager = Scene->GetSceneSubsystem<FCameraManager>();
        FCameraComponent& CameraComponent = CameraManager->GetActiveCameraEntity().GetComponent<FCameraComponent>();

        SceneGlobalData.CameraData.Location =   glm::vec4(CameraComponent.GetPosition(), 1.0f);
        SceneGlobalData.CameraData.View =       CameraComponent.GetViewMatrix();
        SceneGlobalData.CameraData.Projection = CameraComponent.GetProjectionMatrix();
        SceneGlobalData.Time =                  (float)glfwGetTime();
        SceneGlobalData.DeltaTime =             (float)Scene->GetSceneDeltaTime();

        SceneViewport->SetViewVolume(CameraComponent.GetViewVolume());

        // Triangle vertex data
        FSimpleElementVertex TriangleVerts[3] =
        {
            { glm::vec4( 1.0f,  1.0f, 0.0f, 1.0f),  glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) },  // Vertex 0 (Red)
            { glm::vec4(-1.0f,  1.0f, 0.0f, 1.0f),  glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) },  // Vertex 1 (Green)
            { glm::vec4( 0.0f, -1.0f, 0.0f, 1.0f),  glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) }   // Vertex 2 (Blue)
        };

        
        FRHIBufferDesc VertexBufferDesc;
        VertexBufferDesc.Size = sizeof(FSimpleElementVertex) * std::size(TriangleVerts);
        VertexBufferDesc.Usage.SetFlag(BUF_VertexBuffer);
        FRHIBufferRef VertexBuffer = RenderContext->CreateBuffer(VertexBufferDesc);

        FVertexAttributeDesc VertexDesc[2];
        // Pos
        VertexDesc[0].SetElementStride(sizeof(FSimpleElementVertex));
        VertexDesc[0].SetOffset(offsetof(FSimpleElementVertex, Position));
        VertexDesc[0].Format = EFormat::RGBA32_FLOAT;

        // Color
        VertexDesc[1].SetElementStride(sizeof(FSimpleElementVertex));
        VertexDesc[1].SetOffset(offsetof(FSimpleElementVertex, Color));
        VertexDesc[1].Format = EFormat::RGBA32_FLOAT;
        
        FRHIInputLayoutRef InputLayout = RenderContext->CreateInputLayout(VertexDesc, 2);

        CommandList->UploadToBuffer(VertexBuffer, TriangleVerts, 0, sizeof(FSimpleElementVertex) * std::size(TriangleVerts));

        FRenderPassBeginInfo BeginInfo; BeginInfo
        .AddColorAttachment(GetRenderTarget())
        .SetColorLoadOp(ERenderLoadOp::Clear)
        .SetColorStoreOp(ERenderLoadOp::Clear)
        .SetRenderArea(GetRenderTarget()->GetExtent())
        .SetColorClearColor(FColor::Black);
        CommandList->BeginRenderPass(BeginInfo);

        FBindingLayoutItem Item;
        Item.Size = sizeof(float);
        Item.Slot = 0;
        Item.Type = ERHIBindingResourceType::Buffer_CBV;
        
        //FBindingLayoutDesc LayoutDesc;
        //FRHIBindingLayoutRef Layout = RenderContext->CreateBindingLayout(LayoutDesc);

        //FBindingSetDesc SetDesc;
        //FRHIBindingSetRef Set = RenderContext->CreateBindingSet(SetDesc, Layout);
        
        FGraphicsPipelineDesc Desc;
        Desc.SetInputLayout(InputLayout);
        //Desc.AddBindingLayout(Layout);
        Desc.SetVertexShader(RenderContext->GetShaderLibrary()->GetShader("Primitive.vert").As<FRHIVertexShader>());
        Desc.SetPixelShader(RenderContext->GetShaderLibrary()->GetShader("Primitive.frag").As<FRHIPixelShader>());
        Desc.SetPrimType(EPrimitiveType::TriangleList);

        FRHIGraphicsPipelineRef Pipeline = RenderContext->CreateGraphicsPipeline(Desc);

        CommandList->SetGraphicsPipeline(Pipeline);

        //CommandList->BindBindingSet(Set, ERHIBindingPoint::Graphics);

        CommandList->SetViewport(0.0f, 0.0f, 0.0f, (float)GetRenderTarget()->GetSizeX(), (float)GetRenderTarget()->GetSizeY(), 1.0f);
        CommandList->SetScissorRect(0, 0, GetRenderTarget()->GetSizeX(), GetRenderTarget()->GetSizeY());

        CommandList->SetRequiredImageAccess(GetRenderTarget(), ERHIAccess::HostRead);

        CommandList->BindVertexBuffer(VertexBuffer, 0, 0);
        CommandList->Draw(3, 1, 0, 0);
        

        CommandList->EndRenderPass();
        
    }

    void FSceneRenderer::EndScene(const FScene* Scene)
    {
        ForwardRenderPass(Scene);
        FullScreenPass(Scene);
    }

    void FSceneRenderer::OnSwapchainResized()
    {

        CreateImages();
        
    }

    void FSceneRenderer::ForwardRenderPass(const FScene* Scene)
    {
    }

    void FSceneRenderer::FullScreenPass(const FScene* Scene)
    {
    }

    void FSceneRenderer::DrawPrimitives(const FScene* Scene)
    {
        
    }
    
    void FSceneRenderer::InitBuffers()
    {
    }
    
    void FSceneRenderer::CreateImages()
    {
    }
    
}
