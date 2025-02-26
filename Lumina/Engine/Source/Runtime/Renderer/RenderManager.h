#pragma once
#include "Subsystems/Subsystem.h"
#include "Lumina.h"

namespace Lumina
{
    class IImGuiRenderer;
    class IRenderContext;
}

namespace Lumina
{
    class FRenderManager : public ISubsystem
    {
    public:

        void Initialize(FSubsystemManager& Manager) override;
        void Deinitialize() override;

        void FrameStart(const FUpdateContext& UpdateContext);
        void FrameEnd(const FUpdateContext& UpdateContext);

        template<typename T>
        T* GetRenderContext() const
        {
            return static_cast<T*>(RenderContext);
        }
        
        FORCEINLINE IRenderContext* GetRenderContext() const { return RenderContext; }
        
    private:

        #if WITH_DEVELOPMENT_TOOLS
        IImGuiRenderer*     ImGuiRenderer =         nullptr;
        #endif
        
        IRenderContext*     RenderContext = nullptr;
        uint8               CurrentFrameIndex = 1;
        
    };
}
