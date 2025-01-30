#include "pch.h"

#include "Application.h"
#include "Core/Layer.h"
#include "Assets/AssetManager/AssetManager.h"
#include "ImGui/ImGuiRenderer.h"
#include "Input/InputSubsystem.h"
#include "Memory/Memory.h"
#include "Memory/MemoryLeakDetection.h"
#include "Renderer/Renderer.h"
#include "Core/Windows/Window.h"
#include "Core/Windows/WindowSubsystem.h"
#include "Core/Windows/WindowTypes.h"
#include "Scene/Scene.h"

namespace Lumina
{

    TMulticastDelegate<void> FCoreDelegates::OnEngineInit;
    TMulticastDelegate<void> FCoreDelegates::PreEngineShutdown;
    TMulticastDelegate<double> FCoreDelegates::OnEngineUpdate;
    
    FApplication* FApplication::Instance = nullptr;

    void FApplicationStats::PreFrame()
    {
        DeltaTime = (CurrentFrameTime - LastFrameTime);

        if (DeltaTime > 0)
        {
            FPS = static_cast<uint32>(1000.0f / DeltaTime);
        }

        FrameCount++;
    }

    void FApplicationStats::PostFrame()
    {
        LastFrameTime =     CurrentFrameTime;
        CurrentFrameTime =  glfwGetTime() * 1000.0f;
    }

    FApplication::FApplication(const FApplicationSpecs& InAppSpecs)
    {
        AppSpecs = InAppSpecs;
        Instance = this;
    }

    FApplication::~FApplication()
    {
        Instance = nullptr;
    }

    void FApplication::Run()
    {
        InternalInit();
        
        while(!ShouldExit())
        {
            if (true)
            {
                PROFILE_SCOPE(ApplicationFrame)
                PreFrame();

                if (CurrentScene)
                {
                    CurrentScene->OnUpdate(Stats.DeltaTime);
                }
                
                ApplicationSubsystems.Update(Stats.DeltaTime);
                
                OnUpdate();
                
                RenderImGui(Stats.DeltaTime);
                
                FRenderer::Render();

                PostFrame();
            }
        }

        InternalShutdown();
    }
    
    void FApplication::InternalInit()
    {
        OnInit();
    }

    void FApplication::InternalShutdown()
    {
        OnShutdown();

        LOG_TRACE("{0} Shutting Down..", AppSpecs.Name);

        bRunning = false;

        LayerStack.DetachAllLayers();

        ApplicationSubsystems.DeinitializeAll();

        if (CurrentScene)
        {
            CurrentScene = nullptr;
        }

        if (AppSpecs.bRenderImGui)
        {
            FImGuiRenderer::Shutdown();
        }
        
        FRenderer::Shutdown();
        AssetManager::Get()->Shutdown();
        
        glfwTerminate();
        
    }

    void FApplication::OnInit()
    {
        LOG_TRACE("Initializing Application: {0}", AppSpecs.Name);
        
        WindowSubsystem* WinSubsystem = ApplicationSubsystems.AddSubsystem<WindowSubsystem>();
        
        /* Create application window */
        FWindowSpecs AppWindowSpecs;
        AppWindowSpecs.Title = AppSpecs.Name;
        
        WinSubsystem->InitializeWindow(AppWindowSpecs);
        WinSubsystem->GetWindow()->SetEventCallback(BIND_EVENT_FN(OnEvent));

        FRenderConfig Config;
        Config.Window = WinSubsystem->GetWindow();
        Config.FramesInFlight = 2;
        FRenderer::Init(Config);
        
        ApplicationSubsystems.AddSubsystem<InputSubsystem>();

        if (AppSpecs.bRenderImGui)
        {
            FImGuiRenderer::Init();
        }

    }

    void FApplication::OnUpdate()
    {
        
    }

    void FApplication::OnShutdown()
    {
    }
    
    void FApplication::PreFrame()
    {
        Stats.PreFrame();
        FRenderer::BeginFrame();
    }

    void FApplication::PostFrame()
    {
        Stats.PostFrame();

        FRenderer::EndFrame();
        
        glfwPollEvents();

    }

    void FApplication::UpdateLayerStack(double DeltaTime)
    {
        PROFILE_SCOPE(LayerStackUpdate)
        for (TRefPtr<FLayer>& Layer : LayerStack)
        {
            Layer->OnUpdate(DeltaTime);    
        }
    }
    
    void FApplication::PushLayer(const TRefPtr<FLayer>& InLayer)
    {
        LayerStack.PushLayer(InLayer);
        InLayer->OnAttach();
    }

    void FApplication::PushOverlay(const TRefPtr<FLayer>& InLayer)
    {
        LayerStack.PushOverlay(InLayer);
        InLayer->OnAttach();
    }

    void FApplication::PopLayer(const TRefPtr<FLayer>& InLayer)
    {
        LayerStack.PopLayer(InLayer);
        InLayer->OnDetach();
    }

    void FApplication::PopOverlay(const TRefPtr<FLayer>& InLayer)
    {
        LayerStack.PopOverlay(InLayer);
        InLayer->OnDetach();
    }

    void FApplication::RenderImGui(double DeltaTime)
    {
        if (AppSpecs.bRenderImGui)
        {
            PROFILE_SCOPE(ImGuiRenderUpdate)
            FImGuiRenderer::BeginFrame();

            for (auto& Layer : LayerStack)
            {
                Layer->ImGuiRender(DeltaTime);    
            }
        
            FImGuiRenderer::EndFrame();
        }
    }

    void FApplication::OnEvent(FEvent& Event)
    {
        for (auto& Layer : LayerStack)
        {
            Layer->OnEvent(Event);
        }
    }

    FWindow& FApplication::GetWindow()
    {
        return *Get().GetSubsystem<WindowSubsystem>()->GetWindow();
    }

    void FApplication::SetCurrentScene(TSharedPtr<LScene> InScene)
    {
        CurrentScene = InScene;
    }

    bool FApplication::ShouldExit()
    {
        return glfwWindowShouldClose(GetSubsystem<WindowSubsystem>()->GetWindow()->GetWindow()) || !bRunning;
    }


}
