#pragma once

#include "Events/KeyCodes.h"
#include "Core/Windows/Window.h"
#include <GLFW/glfw3.h>


namespace Lumina::Input
{
    
    inline bool IsMouseButtonPressed(const KeyCode Key) 
    {
        GLFWwindow* Window = Windowing::GetPrimaryWindowHandle()->GetWindow();
        auto State = glfwGetMouseButton(Window, Key);

        return State == GLFW_PRESS;
    }

    inline bool IsKeyPressed(const KeyCode Key)
    {
        GLFWwindow* Window = Windowing::GetPrimaryWindowHandle()->GetWindow();
        auto State = glfwGetKey(Window, Key);

        return State == GLFW_PRESS;
    }

    inline glm::vec2 GetMousePos()
    {
        GLFWwindow* Window = Windowing::GetPrimaryWindowHandle()->GetWindow();
        double xpos, ypos;
        glfwGetCursorPos(Window, &xpos, &ypos);

        return { (float)xpos, (float)ypos };
    }
    
}
