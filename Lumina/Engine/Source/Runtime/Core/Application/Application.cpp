
#include "Application.h"
#include "Assets/AssetManager/AssetManager.h"
#include "Core/Windows/Window.h"
#include "Core/Windows/WindowTypes.h"

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
        CreateEngine();
        
        if (!Initialize())
        {
            Shutdown();
            return 1;
        }

        FCoreDelegates::OnEngineInit.Broadcast();
        
        //---------------------------------------------------------------
        // Core application loop.
        //--------------------------------------------------------------

        
        bool bExitRequested = false;
        while(!ShouldExit() && !bExitRequested)
        {
            Engine->Update();
            
            bExitRequested = !ApplicationLoop();
            
            Window->ProcessMessages();
        }

        
        //---------------------------------------------------------------
        // Application shutdown.
        //--------------------------------------------------------------

        LOG_TRACE("Shutting down application: {0}", ApplicationName.c_str());

        FCoreDelegates::PreEngineShutdown.Broadcast();
        
        Shutdown();
        
        Engine->Shutdown();
        
        Window->Shutdown();

        delete Window;
        
        return 0;
    }

    bool FApplication::HasAnyFlags(EApplicationFlags Flags)
    {
        return (ApplicationFlags & static_cast<uint32>(Flags)) != 0;
    }
    
    FWindow* FApplication::GetMainWindow()
    {
        return Get().Window;
    }

    void FApplication::WindowResized(const FIntVector2D& Extent)
    {
        Engine->SetEngineViewportSize(Extent);

        OnWindowResized(Extent);
    }


    bool FApplication::CreateApplicationWindow()
    {
        FWindowSpecs AppWindowSpecs;
        AppWindowSpecs.Title = ApplicationName.c_str();
        
        AppWindowSpecs.Context.ResizeCallback = [this] (const FIntVector2D& Extent)
        {
            WindowResized(Extent);
        };

        Window = FWindow::Create(AppWindowSpecs);
        Window->Init();
        
        Windowing::SetPrimaryWindowHandle(Window);
        
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
