#pragma once
#include "Subsystems/Subsystem.h"

namespace Lumina
{
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

        IRenderContext*     RenderContext = nullptr;
        uint8               CurrentFrameIndex = 0;
        
    };
}
