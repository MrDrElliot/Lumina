#pragma once
#include "Core/UpdateContext.h"
#include "Core/Delegates/Delegate.h"
#include "Subsystems/Subsystem.h"


namespace Lumina
{
    class FApplication;
    class FWindow;

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

        virtual void DrawDevelopmentTools();
    
    protected:

        virtual void PostInitialize() { }

        virtual void PreShutdown() { }


    private:

        void CreateDevelopmentUITools();

        FUpdateContext      UpdateContext;
        FApplication*       Application =       nullptr;
        FSubsystemManager*  EngineSubsystems =  nullptr;
        
    };
}
