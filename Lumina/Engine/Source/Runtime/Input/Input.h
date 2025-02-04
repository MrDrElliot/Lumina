#pragma once
#include "Source/Runtime/Events/KeyCodes.h"
#include "glm/glm.hpp"
#include "Core/Windows/Window.h"
#include <Core/Application/Application.h>
#include <GLFW/glfw3.h>


namespace Lumina::Input
{
    
    inline bool IsMouseButtonPressed(const KeyCode Key) 
    {
        GLFWwindow* Window = FApplication::Get().GetWindow()->GetWindow();
        auto State = glfwGetMouseButton(Window, Key);

        return State == GLFW_PRESS;
    }

    inline bool IsKeyPressed(const KeyCode Key)
    {
        GLFWwindow* Window = FApplication::Get().GetWindow()->GetWindow();
        auto State = glfwGetKey(Window, Key);

        return State == GLFW_PRESS;
    }

    inline glm::vec2 GetMousePos()
    {
        GLFWwindow* Window = FApplication::Get().GetWindow()->GetWindow();
        double xpos, ypos;
        glfwGetCursorPos(Window, &xpos, &ypos);

        return { (float)xpos, (float)ypos };
    }
    
}
