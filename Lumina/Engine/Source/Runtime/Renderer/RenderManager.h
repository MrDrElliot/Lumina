#pragma once
#include "Subsystems/Subsystem.h"

namespace Lumina
{
    class IRenderDevice;
}

namespace Lumina
{
    class IRenderBackend;
}

namespace Lumina
{
    class FRenderManager : public ISubsystem
    {
    public:

        void Initialize(const FSubsystemManager& Manager) override;
        void Deinitialize() override;

        void FrameStart();
        void FrameEnd();

    private:

        IRenderBackend*     RenderBackend = nullptr;
    
    };
}
