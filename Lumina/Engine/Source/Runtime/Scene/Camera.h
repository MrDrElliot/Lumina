#pragma once

#include <glm/glm.hpp>

#include "Source/Runtime/Events/Event.h"

namespace Lumina
{
    class FCamera
    {
    public:

        FCamera();
        ~FCamera();

        glm::mat4 GetCameraProject();
        glm::mat4 GetViewMatrix();
        glm::mat4 GetRotationMatrix();

        void OnEvent(FEvent& Event);

        void Update(float DeltaTime);
        void UpdateCameraVectors();
        void SetAspectRatio(float Ratio) { AspectRatio = Ratio; }


    private:
        
        glm::vec3 Velocity;
        glm::vec3 Position;
        
        float Pitch = 0.0f;
        float Yaw = 0.0f;
        float AspectRatio = 16.0/8.0f;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
    };
}
