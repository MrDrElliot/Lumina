#pragma once

#include "Subsystems/Subsystem.h"
#include <glm/glm.hpp>

namespace Lumina
{
    class FInputSubsystem : public ISubsystem
    {
    public:

        void Initialize(const FSubsystemManager& Manager) override;
        void Update(const FUpdateContext& UpdateContext);
        void Deinitialize() override;

        FORCEINLINE float GetMouseDeltaYaw() const { return MouseDeltaYaw; }
        FORCEINLINE float GetMouseDeltaPitch() const { return MouseDeltaPitch; }

    private:

        float MouseDeltaYaw =    0.0f;
        float MouseDeltaPitch =  0.0f;
        glm::vec2 MousePosLastFrame = glm::vec2(0.0f);
    
    };
}
