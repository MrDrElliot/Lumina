#pragma once
#include "InputSubsystem.h"
#include "Events/KeyCodes.h"

namespace Lumina
{
    namespace Input
    {
        inline bool IsKeyPressed(KeyCode Key)
        {
            return GEngine->GetEngineSubsystem<FInputSubsystem>()->IsKeyPressed(Key);
        }

        inline bool IsMouseButtonPressed(MouseCode Button)
        {
            return GEngine->GetEngineSubsystem<FInputSubsystem>()->IsMouseButtonPressed(Button);
        }

        inline glm::vec2 GetMousePosition()
        {
            return GEngine->GetEngineSubsystem<FInputSubsystem>()->GetMousePosition();
        }

        inline float GetMouseDeltaPitch()
        {
            return GEngine->GetEngineSubsystem<FInputSubsystem>()->GetMouseDeltaPitch();
        }

        inline float GetMouseDeltaYaw()
        {
            return GEngine->GetEngineSubsystem<FInputSubsystem>()->GetMouseDeltaYaw();
        }
    }
}
