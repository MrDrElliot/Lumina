
#include "InputSubsystem.h"

#include "Input.h"
#include "Core/Profiler/Profile.h"

namespace Lumina
{
    void FInputSubsystem::Initialize(FSubsystemManager& Manager)
    {
    }

    void FInputSubsystem::Update(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();

        glm::vec2 MousePos = Input::GetMousePos();
        if (MousePosLastFrame == glm::vec2(0.0f))
        {
            MousePosLastFrame = MousePos;
            return;
        }
        
        MouseDeltaPitch = MousePos.y - MousePosLastFrame.y;
        MouseDeltaYaw = MousePos.x - MousePosLastFrame.x;
        
        
        MousePosLastFrame = MousePos;
    }

    void FInputSubsystem::Deinitialize()
    {
    }
}
