#include "Camera.h"

#include "ScenePrimitives.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "Source/Runtime/Events/MouseCodes.h"
#include "Source/Runtime/Input/Input.h"
#include "Source/Runtime/Renderer/Buffer.h"
#include "Source/Runtime/Renderer/Renderer.h"


namespace Lumina
{
    FCamera::FCamera()
    {
        Position = {0.0f, 1.5f, 9.0f};
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

    TSharedPtr<FCamera> FCamera::Create()
    {
        return MakeSharedPtr<FCamera>();
    }

    glm::mat4 FCamera::GetViewProjectionMatrix()
    {
        glm::mat4 view = GetViewMatrix();
        glm::mat4 projection = GetProjectionMatrix();
        
        return projection * view;
    }

    glm::mat4 FCamera::GetViewMatrix()
    {
        glm::mat4 View = glm::lookAt(Position, Position + Front, Up);
        return View;
    }

    glm::mat4 FCamera::GetProjectionMatrix()
    {
        /* zNear and zFar are flipped to use reverse depth */
        glm::mat4 Projection = glm::perspective(glm::radians(FOV), AspectRatio, 10000.0f, 0.01f);
        return Projection;
    }

    glm::mat4 FCamera::GetProjectionMatrixNoReverse()
    {
        /* zNear and zFar are flipped to use reverse depth */
        glm::mat4 Projection = glm::perspective(glm::radians(FOV), AspectRatio, 0.01f, 10000.0f);
        return Projection;
    }

    void FCamera::Destroy()
    {
    }

    void FCamera::OnEvent(FEvent& Event)
    {
    }

    void FCamera::Rotate(float YawOffset, float PitchOffset, float RollOffset, bool LockPitch)
    {
        Yaw += YawOffset;

        // If pitch is locked (must be in -89.0f to 89.0f range) AND pitch is less than -89.0f OR greater than 89.0f
        if (LockPitch)
        {
            if (Pitch + PitchOffset> 89.0f)
            {
                Pitch = 89.0f;
            }
            else if (Pitch + PitchOffset < -89.0f)
            {
                Pitch = -89.0f;
            }
            else
            {
                Pitch += PitchOffset;
            }
        }
        else
        {
            Pitch += PitchOffset;
        }

      //  Roll += rollOffset;

        UpdateCameraVectors();
    }

    void FCamera::Move(glm::vec3 Direction)
    {
        Position += Right * Direction.x;
        Position += Up * Direction.y;
        Position += Front * Direction.z;

    }

    void FCamera::Update(double DeltaTime)
    {
        if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
        {
            glm::vec2 MousePos = Input::GetMousePos();

            if (bFirstMouse)
            {
                LastMousePos = MousePos;
                bFirstMouse = false;
            }

            glm::vec2 MouseDelta = MousePos - LastMousePos;
            LastMousePos = MousePos;  // Update the last mouse position

            Yaw += MouseDelta.x * 0.4f;
            Pitch = std::clamp(Pitch - MouseDelta.y * 0.4f, -89.9f, 89.9f);
        }
        else
        {
            bFirstMouse = true;  // Reset the first-time flag when the mouse button is not pressed
        }

        float velocity = 0.01f * (float)DeltaTime;
        if(Input::IsKeyPressed(Key::LeftShift))
        {
            velocity *= 4;
        }
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
