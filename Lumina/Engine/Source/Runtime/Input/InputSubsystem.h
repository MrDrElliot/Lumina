#pragma once

#include "Subsystems/Subsystem.h"

namespace Lumina
{
    class FInputSubsystem : public ISubsystem
    {
    public:

        void Initialize() override;
        void Update(double DeltaTime) override;
        void Deinitialize() override;
    
    };
}
