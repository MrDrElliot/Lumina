
#include "InputSubsystem.h"
#include "Core/Profiler/Profile.h"
#include "Core/Windows/Window.h"

namespace Lumina
{
    void FInputSubsystem::Initialize()
    {
    }

    void FInputSubsystem::Update(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();

        FWindow* PrimaryWindow = Windowing::GetPrimaryWindowHandle();
        GLFWwindow* Window = PrimaryWindow->GetWindow();

        int Mode = glfwGetInputMode(Window, GLFW_CURSOR);
        int DesiredMode = Snapshot.CursorMode.load(std::memory_order::memory_order_relaxed);
        if (Mode != DesiredMode)
        {
            glfwSetInputMode(Window, GLFW_CURSOR, DesiredMode);
        }


        double xpos, ypos;
        glfwGetCursorPos(Window, &xpos, &ypos);
        glm::vec2 MousePos = {xpos, ypos};
        if (MousePosLastFrame == glm::vec2(0.0f))
        {
            MousePosLastFrame = MousePos;
            return;
        }
        
        MouseDeltaPitch = MousePos.y - MousePosLastFrame.y;
        MouseDeltaYaw = MousePos.x - MousePosLastFrame.x;
        

        // Keys
        for (int k = Key::Space; k < Key::Num; ++k)
        {
            Snapshot.Keys[k].store(glfwGetKey(Window, k) == GLFW_PRESS, std::memory_order_relaxed);
        }

        // Mouse
        for (int b = 0; b < Mouse::Num; ++b)
        {
            Snapshot.MouseButtons[b].store(glfwGetMouseButton(Window, b) == GLFW_PRESS, std::memory_order_relaxed);
        }

        // Cursor
        Snapshot.MouseX.store(static_cast<float>(xpos), std::memory_order_relaxed);
        Snapshot.MouseY.store(static_cast<float>(ypos), std::memory_order_relaxed);
        
        MousePosLastFrame = MousePos;
    }

    void FInputSubsystem::Deinitialize()
    {
    }
    

    void FInputSubsystem::SetCursorMode(int NewMode)
    {
        Snapshot.CursorMode.store(NewMode, std::memory_order_relaxed);
    }

    bool FInputSubsystem::IsKeyPressed(KeyCode Key)
    {
        return Snapshot.Keys[Key].load(std::memory_order_relaxed);
    }

    bool FInputSubsystem::IsMouseButtonPressed(MouseCode Button)
    {
        return Snapshot.MouseButtons[Button].load(std::memory_order_relaxed);
    }

    glm::vec2 FInputSubsystem::GetMousePosition() const
    {
        return { Snapshot.MouseX.load(std::memory_order_relaxed), Snapshot.MouseY.load(std::memory_order_relaxed) };
    }

}

