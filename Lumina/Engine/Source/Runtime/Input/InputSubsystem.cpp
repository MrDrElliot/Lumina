
#include "InputSubsystem.h"

#include "Input.h"

namespace Lumina
{
    void FInputSubsystem::Initialize(const FSubsystemManager& Manager)
    {
    }

    void FInputSubsystem::Update(const FUpdateContext& UpdateContext)
    {
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
