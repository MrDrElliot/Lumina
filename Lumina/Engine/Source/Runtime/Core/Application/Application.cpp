
#include "Application.h"
#include "tracy/Tracy.hpp"
#include "ApplicationGlobalState.h"
#include "Assets/AssetManager/AssetManager.h"
#include "Core/Module/ModuleManager.h"
#include "Core/Object/ObjectBase.h"
#include "Core/Object/ObjectCore.h"
#include "Core/Utils/CommandLineParser.h"
#include "Core/Windows/Window.h"
#include "Core/Windows/WindowTypes.h"
#include "Paths/Paths.h"
#include "Platform/Process/PlatformProcess.h"
#include "Project/Project.h"

namespace Lumina
{

    TMulticastDelegate<void> FCoreDelegates::OnEngineInit;
    TMulticastDelegate<void> FCoreDelegates::PreEngineShutdown;
    TMulticastDelegate<double> FCoreDelegates::OnEngineUpdate;
    
    FApplication* FApplication::Instance = nullptr;

    FApplication::FApplication(const FString& InApplicationName, uint32 AppFlags)
    {
        ApplicationName = InApplicationName;
        ApplicationFlags = AppFlags;
        Instance = this;
    }

    int32 FApplication::Run(int argc, char** argv)
    {
        FrameMarkNamed("main");
        LOG_TRACE("Initializing Application: {0}", ApplicationName.c_str());

        //---------------------------------------------------------------
        // Application initialization.
        //--------------------------------------------------------------

        PreInitStartup(argc, argv);
        CreateApplicationWindow();
        CreateEngine();

        
        if (!Initialize(argc, argv))
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
            GEngine->Update();
            
            bExitRequested = !ApplicationLoop();
            
            Window->ProcessMessages();
        }

        
        //---------------------------------------------------------------
        // Application shutdown.
        //--------------------------------------------------------------

        LOG_TRACE("Shutting down application: {0}", ApplicationName.c_str());

        FCoreDelegates::PreEngineShutdown.Broadcast();
        
        Shutdown();
        
        GEngine->Shutdown();
        
        Window->Shutdown();
        Memory::Delete(Window);

        FModuleManager::Get()->UnloadAllModules();
        
        return 0;
    }

    bool FApplication::HasAnyFlags(EApplicationFlags Flags)
    {
        return (ApplicationFlags & static_cast<uint32>(Flags)) != 0;
    }

    void FApplication::WindowResized(const FIntVector2D& Extent)
    {
        GEngine->SetEngineViewportSize(Extent);

        OnWindowResized(Extent);
    }


    void FApplication::PreInitStartup(int argc, char** argv)
    {
        Paths::Mount("engine://", Paths::GetEngineContentDirectory());
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
