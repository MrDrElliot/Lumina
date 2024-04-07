#include "Camera.h"

#include "glm/detail/type_quat.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/gtx/quaternion.hpp"

namespace Lumina
{
    void FCamera::Update(float DeltaTime)
    {
        glm::mat4 CameraRotation = GetRotationMatrix();
        Position += glm::vec3(CameraRotation * glm::vec4(Velocity * 0.5f, 0.0f));
    }

    glm::mat4 FCamera::GetViewMatrix()
    {
        glm::mat4 CameraTranslation = glm::translate(glm::mat4(1.0f), Position);
        glm::mat4 CameraRotation = GetRotationMatrix();

        return glm::inverse(CameraTranslation * CameraRotation);
    }

    glm::mat4 FCamera::GetRotationMatrix()
    {
        glm::quat PitchRotation = glm::angleAxis(Pitch, glm::vec3{1.0f, 0.0f, 0.0f });
        glm::quat YawRotation = glm::angleAxis(Yaw, glm::vec3{ 0.0f, -1.0f, 0.0f});

        return glm::toMat4(YawRotation) * glm::toMat4(PitchRotation);
    }
}
