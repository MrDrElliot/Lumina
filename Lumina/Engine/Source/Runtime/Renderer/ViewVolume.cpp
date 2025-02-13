#include "ViewVolume.h"

namespace Lumina
{
    FViewVolume::FViewVolume(float fov, float aspect, float nearPlane, float farPlane)
        : FOV(fov), AspectRatio(aspect), DepthRange(nearPlane, farPlane)
    {
        ViewPosition =  glm::vec3(1.0f);
        RightVector =   glm::vec3(1.0f, 0.0f, 0.0f);
        UpVector =      glm::vec3(0.0f, 1.0f, 0.0f);
        ForwardVector = glm::vec3(0.0f, 0.0f, -1.0f);
        RightVector =   glm::normalize(glm::cross(ForwardVector, UpVector));

        if (DepthRange.x <= 0.0f || DepthRange.x >= DepthRange.y)
        {
            DepthRange = glm::vec2(0.1f, 1000.0f);
        }
        
        SetPerspective(fov, aspect, nearPlane, farPlane);
    }


    void FViewVolume::SetViewPosition(const glm::vec3& Position)
    {
        ViewPosition = Position;
        UpdateMatrices();
    }
    
    void FViewVolume::SetView(const glm::vec3& Position, const glm::vec3& ViewDirection, const glm::vec3& UpDirection)
    {
        ViewPosition = Position;
        ForwardVector = glm::normalize(ViewDirection - Position);
        RightVector = glm::normalize(glm::cross(ForwardVector, glm::normalize(UpDirection)));
        UpVector = glm::normalize(glm::cross(RightVector, ForwardVector));

        UpdateMatrices();
    }


    void FViewVolume::SetPerspective(float fov, float aspect, float nearPlane, float farPlane)
    {
        FOV = fov;
        AspectRatio = aspect;
        DepthRange = glm::vec2(nearPlane, farPlane);

        ProjectionMatrix = glm::perspective(glm::radians(FOV), AspectRatio, nearPlane, farPlane);
        UpdateMatrices();
    }


    void FViewVolume::SetAspectRatio(float InAspect)
    {
        AspectRatio = InAspect;

        ProjectionMatrix = glm::perspective(glm::radians(FOV), AspectRatio, GetNearPlane(), GetFarPlane());
        UpdateMatrices();
    }

    void FViewVolume::SetDepthRange(const glm::vec2& InRange)
    {
        DepthRange = InRange;
        SetPerspective(FOV, AspectRatio, InRange.x, InRange.y);
    }
    
    void FViewVolume::SetFOV(float InFOV)
    {
        FOV = InFOV;

        ProjectionMatrix = glm::perspective(glm::radians(FOV), AspectRatio, GetNearPlane(), GetFarPlane());
        UpdateMatrices();
    }

    void FViewVolume::UpdateMatrices()
    {
        ViewMatrix = glm::lookAt(ViewPosition, ViewPosition + ForwardVector, UpVector);
        ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
    }
}
