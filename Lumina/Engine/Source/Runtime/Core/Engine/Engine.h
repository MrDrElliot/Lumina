#pragma once
#include "Lumina.h"
#include "Core/UpdateContext.h"
#include "Core/Delegates/Delegate.h"
#include "Subsystems/Subsystem.h"



namespace Lumina
{
    class IImGuiRenderer;
    class IDevelopmentToolUI;
    class FSceneManager;
    class FAssetManager;
    class FInputSubsystem;
    class FApplication;
    class FWindow;
    class FSceneRenderer;
}

namespace Lumina
{
    struct FCoreDelegates
    {
        static TMulticastDelegate<void>		OnEngineInit;
        static TMulticastDelegate<double>	OnEngineUpdate;
        static TMulticastDelegate<void>		PreEngineShutdown;
    };
    
    class FEngine
    {
    public:
        
        virtual ~FEngine() = default;

        bool Initialize(FApplication* App);
        bool Shutdown();
        bool Update();

        virtual void CreateDevelopmentTools() = 0;
        virtual void DrawDevelopmentTools();
    
    protected:

        virtual void PostInitialize() { };
        virtual void PreShutdown() { };


    protected:
        
        FUpdateContext          UpdateContext;
        FApplication*           Application =           nullptr;

        #if WITH_DEVELOPMENT_TOOLS
        IDevelopmentToolUI*     DeveloperToolUI =       nullptr;
        IImGuiRenderer*         ImGuiRenderer =         nullptr;
        #endif

        FSubsystemManager       EngineSubsystems;
        FInputSubsystem*        InputSubsystem =        nullptr;
        FAssetManager*          AssetManagerSubystem =  nullptr;
        FSceneManager*          SceneManager =          nullptr;

    };
}
