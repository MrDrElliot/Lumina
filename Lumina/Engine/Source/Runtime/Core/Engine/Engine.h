#pragma once
#include "Lumina.h"
#include "Core/UpdateContext.h"
#include "Core/Delegates/Delegate.h"
#include "Renderer/Viewport.h"
#include "Subsystems/Subsystem.h"


namespace Lumina
{
    class FRenderManager;
}

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

    inline FEngine* GEngine = nullptr;
    
    struct FCoreDelegates
    {
        static TMulticastDelegate<void>		OnEngineInit;
        static TMulticastDelegate<double>	OnEngineUpdate;
        static TMulticastDelegate<void>		PreEngineShutdown;
    };
    
    class FEngine
    {
    public:
        
        FEngine() = default;
        virtual ~FEngine() = default;

        bool Initialize(FApplication* App);
        bool Shutdown();
        bool Update();

        virtual void CreateDevelopmentTools() = 0;
        virtual void DrawDevelopmentTools();

        const FViewport& GetEngineViewport() const { return EngineViewport; }

        void SetUpdateCallback(TFunction<void()> Callback) { UpdateCallback = Callback; }

        void SetEngineViewportSize(const FIntVector2D& InSize);
        
    protected:

        virtual void PostInitialize() { }
        virtual void PreShutdown() { }

    protected:

        TFunction<void()>       UpdateCallback;
        
        FUpdateContext          UpdateContext;
        FApplication*           Application =           nullptr;

        #if WITH_DEVELOPMENT_TOOLS
        IDevelopmentToolUI*     DeveloperToolUI =       nullptr;
        #endif

        FSubsystemManager       EngineSubsystems;
        FInputSubsystem*        InputSubsystem =        nullptr;
        FAssetManager*          AssetManagerSubystem =  nullptr;
        FSceneManager*          SceneManager =          nullptr;
        FRenderManager*         RenderManager =         nullptr;

        FViewport               EngineViewport;
    };
}
