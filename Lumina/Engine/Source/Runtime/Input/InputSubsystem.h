#pragma once

#include "Subsystems/Subsystem.h"

namespace Lumina
{
    class InputSubsystem : public ISubsystem
    {
    public:

        void Initialize() override;
        void Update(double DeltaTime) override;
        void Deinitialize() override;
    
    };
}
