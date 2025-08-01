﻿#include "ViewVolume.h"
#include "Renderer/RHIIncl.h"

namespace Lumina
{
    
    FViewVolume::FViewVolume(float fov, float aspect)
        : FOV(fov), AspectRatio(aspect)
    {
        ViewPosition =  glm::vec3(1.0f);
        UpVector =      glm::vec3(0.0f, 1.0f, 0.0f);
        ForwardVector = glm::vec3(0.0f, 0.0f, -1.0f);
        RightVector =   glm::normalize(glm::cross(ForwardVector, UpVector));
        
        SetPerspective(fov, aspect);
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


    void FViewVolume::SetPerspective(float fov, float aspect)
    {
        FOV = fov;
        AspectRatio = aspect;

        ProjectionMatrix = glm::perspective(glm::radians(FOV), AspectRatio, 1000.0f, 0.1f);
        UpdateMatrices();
    }


    void FViewVolume::SetAspectRatio(float InAspect)
    {
        AspectRatio = InAspect;

        ProjectionMatrix = glm::perspective(glm::radians(FOV), AspectRatio, 1000.0f, 0.1f);
        UpdateMatrices();
    }
    
    void FViewVolume::SetFOV(float InFOV)
    {
        FOV = InFOV;
        ProjectionMatrix = glm::perspective(glm::radians(FOV), AspectRatio, 1000.0f, 0.1f);
        UpdateMatrices();
    }

    void FViewVolume::UpdateMatrices()
    {
        ViewMatrix = glm::lookAt(ViewPosition, ViewPosition + ForwardVector, UpVector);
        ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
    }
}
