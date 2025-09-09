#pragma once

#include "Subsystems/Subsystem.h"
#include <glm/glm.hpp>
#include <glfw/glfw3.h>
#include "Events/KeyCodes.h"
#include "Events/MouseCodes.h"

namespace Lumina
{
    class FInputSubsystem : public ISubsystem
    {
    public:

        void Initialize() override;
        void Update(const FUpdateContext& UpdateContext);
        void Deinitialize() override;

        float GetMouseDeltaYaw() const { return MouseDeltaYaw; }
        float GetMouseDeltaPitch() const { return MouseDeltaPitch; }

        void SetCursorMode(int NewMode);

        bool IsKeyPressed(KeyCode Key);
        bool IsMouseButtonPressed(MouseCode Button);
        glm::vec2 GetMousePosition() const;
        
    private:

        float MouseDeltaYaw =    0.0f;
        float MouseDeltaPitch =  0.0f;
        glm::vec2 MousePosLastFrame = glm::vec2(0.0f);

        
        struct FInputSnapshot
        {
            TArray<std::atomic<bool>, (uint32)Key::Num> Keys;
            TArray<std::atomic<bool>, (uint32)Mouse::Num> MouseButtons;
            std::atomic<float> MouseX;
            std::atomic<float> MouseY;
            std::atomic_int    CursorMode = GLFW_CURSOR_NORMAL;
        } Snapshot;
    };
}
