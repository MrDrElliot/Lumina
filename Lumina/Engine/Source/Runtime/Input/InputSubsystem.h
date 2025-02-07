#pragma once

#include "Subsystems/Subsystem.h"

namespace Lumina
{
    class FInputSubsystem : public ISubsystem
    {
    public:

        void Initialize(const FSubsystemManager& Manager) override;
        void Update(const FUpdateContext& UpdateContext);
        void Deinitialize() override;
    
    };
}
