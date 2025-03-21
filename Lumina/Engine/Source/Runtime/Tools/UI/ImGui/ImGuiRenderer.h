#pragma once

#include "imgui.h"
#include "ImGuiX.h"
#include "Subsystems/Subsystem.h"

namespace Lumina
{
    class FRenderManager;
}

namespace Lumina
{
    class IImGuiRenderer
    {
    public:
        
        virtual ~IImGuiRenderer() = default;

        virtual void Initialize(FSubsystemManager& Manager);
        virtual void Deinitialize();
        
        void StartFrame(const FUpdateContext& UpdateContext);
        void EndFrame(const FUpdateContext& UpdateContext);
        
        virtual void OnStartFrame(const FUpdateContext& UpdateContext) = 0;
        virtual void OnEndFrame(const FUpdateContext& UpdateContext) = 0;
    
    protected:

        
    };
}
