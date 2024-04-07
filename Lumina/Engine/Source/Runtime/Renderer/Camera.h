#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>


namespace Lumina
{
    
    class FCamera
    {
    public:

        FCamera() = default;
        virtual ~FCamera() = default;

        virtual void Update(float DeltaTime = 0.0f);
        glm::mat4 GetViewMatrix();
        glm::mat4 GetRotationMatrix();


    private:

        glm::vec3 Velocity;
        glm::vec3 Position;

        float Pitch = 0.0f;
        float Yaw = 0.0f;
    };
}
