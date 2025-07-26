
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

    FApplication::FApplication(FString InApplicationName, uint32 AppFlags)
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

        FModuleManager::Get()->UnloadAllModules();

        ShutdownCObjectSystem();
        
        delete Window;
        
        return 0;
    }

    bool FApplication::HasAnyFlags(EApplicationFlags Flags)
    {
        return (ApplicationFlags & static_cast<uint32>(Flags)) != 0;
    }

    void FApplication::WindowResized(const FIntVector2D& Extent)
    {
        Engine->SetEngineViewportSize(Extent);

        OnWindowResized(Extent);
    }


    void FApplication::PreInitStartup(int argc, char** argv)
    {
        SetGameFromCommandLine(argc, argv);

        FString DLLName = ".dll";
        FString ProjectBinariesPath = Paths::GetEngineBinariesDirectory().generic_string().c_str();

        Platform::PushDLLDirectory(StringUtils::ToWideString(ProjectBinariesPath).c_str());
        FModuleManager::Get()->LoadModule("PropertyCustomization.dll");
        Platform::PopDLLDirectory();

        
        // Must be called after module initialization.
        ProcessNewlyLoadedCObjects();

    }

    void FApplication::SetGameFromCommandLine(int argc, char** argv)
    {
        FCommandLineParser Parser(argc, argv);
        FString ProjectPath = "";
        FString ProjectDirectory = "";
        FString ProjectName = "";
        
        if (Parser.Has("--project"))
        {
            ProjectPath = Parser.Get("--project");
            LOG_DEBUG("Project Path: {0}", ProjectPath);
            ProjectDirectory = Paths::Parent(ProjectPath);
        }
        else
        {
            LOG_WARN("Error finding project from command line: {0}", *argv);
            return;
        }

        ProjectName = Paths::RemoveExtension(Paths::FileName(ProjectPath));

        //@TODO Proper module discovery.
        FString DLLName = ProjectName + ".dll";
        FString ProjectBinariesPath = Paths::Combine(ProjectDirectory.c_str(), "Binaries/Debug-windows-x86_64");

        Platform::PushDLLDirectory(StringUtils::ToWideString(ProjectBinariesPath).c_str());
        IModuleInterface* Module = FModuleManager::Get()->LoadModule(DLLName);
        Platform::PopDLLDirectory();

        if (Module)
        {
            LOG_WARN("Module Successfully Loaded: {0}", ProjectPath);
        }
        else
        {
            LOG_ERROR("Failed to load module {0}", ProjectBinariesPath);
        }
        
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
