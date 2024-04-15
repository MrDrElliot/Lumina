#include "Camera.h"

#include "imgui.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "Source/Runtime/Events/MouseCodes.h"
#include "Source/Runtime/Input/Input.h"

namespace Lumina
{
    FCamera::FCamera()
    {
        Position = glm::vec3(0.0f);
        Velocity = glm::vec3(0.0f);

        Yaw = -90.0f;
        Pitch = 0.0f;
        WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        Front = glm::vec3(0.0f, 0.0f, -1.0f);
        

        UpdateCameraVectors();
    }

    FCamera::~FCamera()
    {
    }

    glm::mat4 FCamera::GetCameraProject()
    {
        glm::mat4 view = GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(70.f), AspectRatio, 10000.f, 0.1f);
        projection[1][1] *= -1;

        return projection * view;
    }

    glm::mat4 FCamera::GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    glm::mat4 FCamera::GetRotationMatrix()
    {
        return glm::perspective(glm::radians(90.0f), 16.0f/9.0f, 0.1f, 100.0f);
    }

    void FCamera::OnEvent(FEvent& Event)
    {
    }

    void FCamera::Update(float DeltaTime)
    {
        if(Input::IsMouseButtonPressed(Mouse::ButtonRight))
        {
            glm::vec2 MousePos = Input::GetMousePos();
            Yaw = MousePos.x * 0.5f;
            Pitch = glm::clamp(MousePos.y * 0.5f, 0.1f, 269.9f);
            
        }

        float velocity = 0.05f;// * deltaTime;
        if(Input::IsKeyPressed(Key::W))
        {
            Position += Front * velocity;
        }
        if(Input::IsKeyPressed(Key::S))
        {
            Position -= Front * velocity;
        }
        if(Input::IsKeyPressed(Key::A))
        {
            Position -= Right * velocity;
        }
        if(Input::IsKeyPressed(Key::D))
        {
            Position += Right * velocity;
        }
        UpdateCameraVectors();
    }

    void FCamera::UpdateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
}
