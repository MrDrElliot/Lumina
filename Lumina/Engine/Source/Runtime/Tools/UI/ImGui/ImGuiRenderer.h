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
    class FLayer;
    class FImageSampler;
    class FImage;
    
    class IImGuiRenderer : public ISubsystem
    {
    public:
        
        IImGuiRenderer() = default;
        virtual ~IImGuiRenderer() override = default;

        void Initialize(FSubsystemManager& Manager) override;
        void Deinitialize() override { }
        
        void StartFrame(FRenderManager* RenderManager);
        void EndFrame(FRenderManager* RenderManager);
        
        virtual void OnStartFrame(FRenderManager* RenderManager) = 0;
        virtual void OnEndFrame(FRenderManager* RenderManager) = 0;
    
    protected:

        
    };
}
