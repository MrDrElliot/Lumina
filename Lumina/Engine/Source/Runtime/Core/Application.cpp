#include "pch.h"

#include "Application.h"
#include "Layer.h"
#include "ImGui/ImGuiRenderer.h"
#include "Input/InputSubsystem.h"
#include "Memory/Memory.h"
#include "Memory/MemoryLeakDetection.h"
#include "Renderer/Renderer.h"
#include "Windows/Window.h"
#include "Windows/WindowSubsystem.h"
#include "Windows/WindowTypes.h"
#include "Scene/Scene.h"

namespace Lumina
{
    FApplication* FApplication::Instance = nullptr;
    static double accumTime = 0.0;

    FApplication::FApplication(const FApplicationSpecs& InAppSpecs)
    {
        AppSpecs = InAppSpecs;
        Instance = this;
    }

    FApplication::~FApplication()
    {
        
    }
     
    void FApplication::Run()
    {
        OnInit();
        
        while(!ShouldExit())
        {
            if (true)
            {
                PreFrame();
            
                mScene->OnUpdate(Stats.DeltaTime);
                ApplicationSubsystems.Update(Stats.DeltaTime);
                RenderImGui(Stats.DeltaTime);
                FRenderer::Render();

                PostFrame();
            }
        }

        OnShutdown();
    }

    int FApplication::Close()
    {
        return 0;
    }

    void FApplication::OnInit()
    {
        FLog::Init();

        LOG_TRACE("Initializing Application: {0}", AppSpecs.Name);
        
        WindowSubsystem* WinSubsystem = ApplicationSubsystems.AddSubsystem<WindowSubsystem>();
        
        /* Create application window */
        FWindowSpecs AppWindowSpecs;
        AppWindowSpecs.Title = AppSpecs.Name;
        AppWindowSpecs.Width = AppSpecs.WindowWidth;
        AppWindowSpecs.Height = AppSpecs.WindowHeight;
        
        WinSubsystem->InitializeWindow(AppWindowSpecs);
        WinSubsystem->GetWindow()->SetEventCallback(BIND_EVENT_FN(OnEvent));

        FRenderConfig Config;
        Config.Window = WinSubsystem->GetWindow();
        Config.FramesInFlight = 2;
        FRenderer::Init(Config);
        
        ApplicationSubsystems.AddSubsystem<InputSubsystem>();

        mScene = std::make_shared<LScene>();

        FImGuiRenderer::Init();
    }

    void FApplication::OnShutdown()
    {
        LOG_TRACE("{0} Shutting Down..", AppSpecs.Name);

        bRunning = false;

        ApplicationSubsystems.DeinitializeAll();
        
        mScene->Shutdown();
        FImGuiRenderer::Shutdown();
        FRenderer::Shutdown();
        AssetManager::Get()->Shutdown();
        
        glfwTerminate();
        
        LOG_TRACE(Memory::GetProgramMemoryAsString());
        
        FLog::Shutdown();
    }
    
    void FApplication::PreFrame()
    {
        Memory::MemoryLeakDetector::PreFrame();
        Stats.DeltaTime = (Stats.CurrentFrameTime - Stats.LastFrameTime);
        Stats.LastFrameTime = Stats.CurrentFrameTime;

        if (Stats.DeltaTime > 0)
        {
            Stats.FPS = static_cast<uint32>(1.0f / Stats.DeltaTime);
        }

        Stats.FrameCount++;
        accumTime += Stats.DeltaTime;
        
        FRenderer::BeginFrame();
    }

    void FApplication::PostFrame()
    {
        Stats.CurrentFrameTime = glfwGetTime();

        FRenderer::EndFrame();
        
        glfwPollEvents();

        Memory::MemoryLeakDetector::PostFrame();
    }

    void FApplication::UpdateLayerStack(double DeltaTime)
    {
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
        FImGuiRenderer::BeginFrame();

        for (auto& Layer : LayerStack)
        {
            Layer->ImGuiRender(DeltaTime);    
        }
        
        FImGuiRenderer::EndFrame();
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

    bool FApplication::ShouldExit()
    {
        return glfwWindowShouldClose(GetSubsystem<WindowSubsystem>()->GetWindow()->GetWindow()) || !bRunning;
    }
}
