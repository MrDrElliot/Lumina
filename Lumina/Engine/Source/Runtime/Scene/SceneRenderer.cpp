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
        
        FCameraManager* CameraManager = Scene->GetSceneSubsystem<FCameraManager>();
        FCameraComponent& CameraComponent = CameraManager->GetActiveCameraEntity().GetComponent<FCameraComponent>();

        SceneGlobalData.CameraData.Location =   glm::vec4(CameraComponent.GetPosition(), 1.0f);
        SceneGlobalData.CameraData.View =       CameraComponent.GetViewMatrix();
        SceneGlobalData.CameraData.Projection = CameraComponent.GetProjectionMatrix();
        SceneGlobalData.Time =                  (float)glfwGetTime();
        SceneGlobalData.DeltaTime =             (float)Scene->GetSceneDeltaTime();

        SceneViewport->SetViewVolume(CameraComponent.GetViewVolume());

        FColor Color;
        Color.B = glm::sin(SceneGlobalData.Time);
        Color.R = glm::cos(SceneGlobalData.Time);
        
        GEngine->GetEngineSubsystem<FRenderManager>()->GetRenderContext()->GetCommandList()->ClearImageColor(SceneViewport->GetRenderTarget(), Color);

        
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
