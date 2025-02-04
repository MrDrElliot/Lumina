#include "Application.h"
#include "Assets/AssetManager/AssetManager.h"
#include "Core/Performance/PerformanceTracker.h"
#include "ImGui/ImGuiRenderer.h"
#include "Renderer/Renderer.h"
#include "Core/Windows/Window.h"
#include "Core/Windows/WindowTypes.h"
#include "Scene/Scene.h"

namespace Lumina
{

    TMulticastDelegate<void> FCoreDelegates::OnEngineInit;
    TMulticastDelegate<void> FCoreDelegates::PreEngineShutdown;
    TMulticastDelegate<double> FCoreDelegates::OnEngineUpdate;
    
    FApplication* FApplication::Instance = nullptr;

    FApplication::FApplication(FString InApplicationName, uint32 AppFlags)
    {
        ApplicationName = InApplicationName;
        ApplicationFlags = AppFlags;
        Instance = this;
    }

    FApplication::~FApplication()
    {
        Instance = nullptr;
    }

    int32 FApplication::Run()
    {
        LOG_TRACE("Initializing Application: {0}", ApplicationName.c_str());

        //---------------------------------------------------------------
        // Application initialization.
        //--------------------------------------------------------------
        
        CreateApplicationWindow();
        Engine = InitializeEngine();
        
        if (!Initialize())
        {
            Shutdown();
            return 1;
        }
        
        
        //---------------------------------------------------------------
        // Core application loop.
        //--------------------------------------------------------------

        
        bool bExitRequested = false;
        while(!ShouldExit() && !bExitRequested)
        {
            PROFILE_SCOPE(ApplicationFrame)
            
            bExitRequested = !ApplicationLoop();
            
            Window->ProcessMessages();
        }

        
        //---------------------------------------------------------------
        // Application shutdown.
        //--------------------------------------------------------------

        LOG_TRACE("Shutting down application: {0}", ApplicationName.c_str());
        
        Shutdown();
        
        ApplicationSubsystems.DeinitializeAll();

        Engine->Shutdown();
        
        Window->Shutdown();

        delete Window;
        
        return 0;
    }

    bool FApplication::HasAnyFlags(EApplicationFlags Flags)
    {
        return ApplicationFlags & static_cast<uint32>(Flags) != 0;
    }
    
    FWindow* FApplication::GetWindow()
    {
        return Get().Window;
    }

    FEngine* FApplication::InitializeEngine()
    {
        FEngine* NewEngine = new FEngine();
        NewEngine->Initialize(this);
        return NewEngine;
    }
    
    bool FApplication::CreateApplicationWindow()
    {
        FWindowSpecs AppWindowSpecs;
        AppWindowSpecs.Title = ApplicationName.c_str();

        Window = FWindow::Create(AppWindowSpecs);
        Window->Init();
        
        Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

        return true;
    }

    
    bool FApplication::FatalError(const FString& Error)
    {
        return false;
    }

    bool FApplication::ShouldExit()
    {
        return glfwWindowShouldClose(Window->GetWindow());
    }


}
