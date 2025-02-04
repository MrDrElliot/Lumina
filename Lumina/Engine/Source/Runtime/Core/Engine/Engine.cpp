#include "Engine.h"

#include "Assets/AssetManager/AssetManager.h"
#include "glfw/glfw3.h"
#include "ImGui/ImGuiRenderer.h"
#include "Input/InputSubsystem.h"
#include "Renderer/Renderer.h"

namespace Lumina
{
    bool FEngine::Initialize(FApplication* App)
    {
        //-------------------------------------------------------------------------
        // Initialize core engine state.
        //-------------------------------------------------------------------------
        
        Application = App;
        EngineSubsystems = new FSubsystemManager;
        EngineSubsystems->AddSubsystem<FInputSubsystem>();
        EngineSubsystems->AddSubsystem<FAssetManager>();

        UpdateContext.SubsystemManager = EngineSubsystems;
        
        FRenderConfig Config;
        Config.Window = App->GetWindow();
        Config.FramesInFlight = 2;
        
        FRenderer::Init(Config);

        if (App->HasAnyFlags(EApplicationFlags::DevelopmentTools))
        {
            CreateDevelopmentUITools();
        }
        
        PostInitialize();
        
        return true;
    }

    bool FEngine::Shutdown()
    {
        PreShutdown();

        
        if (Application->HasAnyFlags(EApplicationFlags::DevelopmentTools))
        {
            FImGuiRenderer::Shutdown();
        }

        FRenderer::Shutdown();
        return false;
    }

    bool FEngine::Update()
    {
        UpdateContext.MarkFrameStart();
        double DeltaTime = UpdateContext.GetDeltaTime();
        
        EngineSubsystems->GetSubsystem<FInputSubsystem>()->Update(DeltaTime);
        EngineSubsystems->GetSubsystem<FAssetManager>()->Update(DeltaTime);
        

        DrawDevelopmentTools();
        FRenderer::Update();

        UpdateContext.MarkFrameEnd(glfwGetTime());
        
        return false;
    }

    void FEngine::DrawDevelopmentTools()
    {
        if (Application->HasAnyFlags(EApplicationFlags::DevelopmentTools))
        {
            FImGuiRenderer::BeginFrame();
            Application->RenderImGui(UpdateContext.GetDeltaTime());
            FImGuiRenderer::EndFrame();
        }
    }

    void FEngine::CreateDevelopmentUITools()
    {
        FImGuiRenderer::Init();
    }
}
