#pragma once

#include "Module/API.h"
#include "Lumina.h"
#include "Core/UpdateContext.h"
#include "Core/Delegates/Delegate.h"
#include "Renderer/RHIFwd.h"
#include "Renderer/RHIIncl.h"
#include "Subsystems/Subsystem.h"


namespace Lumina
{
    class FAssetRegistry;
    class FRenderManager;
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
    
    class LUMINA_API FEngine
    {
    public:
        
        FEngine() = default;
        virtual ~FEngine() = default;

        bool Initialize(FApplication* App);
        bool Shutdown();
        bool Update();

        virtual void CreateDevelopmentTools() = 0;
        virtual void DrawDevelopmentTools();

        const FRHIViewportRef& GetEngineViewport() const { return EngineViewport; }

        void SetUpdateCallback(TFunction<void(const FUpdateContext&)> Callback) { UpdateCallback = Callback; }

        void SetEngineViewportSize(const FIntVector2D& InSize);

        #if WITH_DEVELOPMENT_TOOLS
        IDevelopmentToolUI* GetDevelopmentToolsUI() const { return DeveloperToolUI; }
        #endif

        template<typename T>
        T* GetEngineSubsystem()
        {
            return EngineSubsystems.GetSubsystem<T>();
        }
        
    protected:

        virtual void PostInitialize() { }
        virtual void PreShutdown() { }

    protected:

        TFunction<void(const FUpdateContext&)>       UpdateCallback;
        
        FUpdateContext          UpdateContext;
        FApplication*           Application =           nullptr;

        #if WITH_DEVELOPMENT_TOOLS
        IDevelopmentToolUI*     DeveloperToolUI =       nullptr;
        #endif

        FSubsystemManager       EngineSubsystems;
        FInputSubsystem*        InputSubsystem =        nullptr;
        FAssetManager*          AssetManager =          nullptr;
        FAssetRegistry*         AssetRegistry =         nullptr;
        FSceneManager*          SceneManager =          nullptr;
        FRenderManager*         RenderManager =         nullptr;

        FRHIViewportRef         EngineViewport;
    };

    extern LUMINA_API FEngine* GEngine;

}
