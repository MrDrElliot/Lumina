#include "ViewVolume.h"
#include "Renderer/RHIIncl.h"

namespace Lumina
{

    glm::vec3 FViewVolume::UpAxis       = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 FViewVolume::ForwardAxis  = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 FViewVolume::RightAxis    = glm::vec3(1.0f, 0.0f, 0.0f);
    
    FViewVolume::FViewVolume(float fov, float aspect)
        : FOV(fov), AspectRatio(aspect)
    {
        ViewPosition =  glm::vec3(1.0f);
        UpVector =      UpAxis;
        ForwardVector = ForwardAxis;
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
        InverseProjectionMatrix = glm::inverse(ProjectionMatrix);
        UpdateMatrices();
    }


    void FViewVolume::SetAspectRatio(float InAspect)
    {
        AspectRatio = InAspect;

        ProjectionMatrix = glm::perspective(glm::radians(FOV), AspectRatio, 1000.0f, 0.1f);
        InverseProjectionMatrix = glm::inverse(ProjectionMatrix);
        UpdateMatrices();
    }
    
    void FViewVolume::SetFOV(float InFOV)
    {
        FOV = InFOV;
        ProjectionMatrix = glm::perspective(glm::radians(FOV), AspectRatio, 1000.0f, 0.1f);
        InverseProjectionMatrix = glm::inverse(ProjectionMatrix);
        UpdateMatrices();
    }

    FFrustum FViewVolume::GetFrustum() const
    {
        FFrustum Frustum;
        const glm::mat4& matrix = ViewProjectionMatrix;

        Frustum.Planes[FFrustum::LEFT].x = matrix[0].w + matrix[0].x;
        Frustum.Planes[FFrustum::LEFT].y = matrix[1].w + matrix[1].x;
        Frustum.Planes[FFrustum::LEFT].z = matrix[2].w + matrix[2].x;
        Frustum.Planes[FFrustum::LEFT].w = matrix[3].w + matrix[3].x;

        Frustum.Planes[FFrustum::RIGHT].x = matrix[0].w - matrix[0].x;
        Frustum.Planes[FFrustum::RIGHT].y = matrix[1].w - matrix[1].x;
        Frustum.Planes[FFrustum::RIGHT].z = matrix[2].w - matrix[2].x;
        Frustum.Planes[FFrustum::RIGHT].w = matrix[3].w - matrix[3].x;

        Frustum.Planes[FFrustum::TOP].x = matrix[0].w - matrix[0].y;
        Frustum.Planes[FFrustum::TOP].y = matrix[1].w - matrix[1].y;
        Frustum.Planes[FFrustum::TOP].z = matrix[2].w - matrix[2].y;
        Frustum.Planes[FFrustum::TOP].w = matrix[3].w - matrix[3].y;

        Frustum.Planes[FFrustum::BOTTOM].x = matrix[0].w + matrix[0].y;
        Frustum.Planes[FFrustum::BOTTOM].y = matrix[1].w + matrix[1].y;
        Frustum.Planes[FFrustum::BOTTOM].z = matrix[2].w + matrix[2].y;
        Frustum.Planes[FFrustum::BOTTOM].w = matrix[3].w + matrix[3].y;

        Frustum.Planes[FFrustum::BACK].x = matrix[0].w + matrix[0].z;
        Frustum.Planes[FFrustum::BACK].y = matrix[1].w + matrix[1].z;
        Frustum.Planes[FFrustum::BACK].z = matrix[2].w + matrix[2].z;
        Frustum.Planes[FFrustum::BACK].w = matrix[3].w + matrix[3].z;

        Frustum.Planes[FFrustum::FRONT].x = matrix[0].w - matrix[0].z;
        Frustum.Planes[FFrustum::FRONT].y = matrix[1].w - matrix[1].z;
        Frustum.Planes[FFrustum::FRONT].z = matrix[2].w - matrix[2].z;
        Frustum.Planes[FFrustum::FRONT].w = matrix[3].w - matrix[3].z;

        for (auto i = 0; i < FFrustum::NUM; i++)
        {
            float length = glm::sqrt(Frustum.Planes[i].x * Frustum.Planes[i].x + Frustum.Planes[i].y * Frustum.Planes[i].y + Frustum.Planes[i].z * Frustum.Planes[i].z);
            Frustum.Planes[i] /= length;
        }

        return Frustum;
    }
    
    void FViewVolume::UpdateMatrices()
    {
        ViewMatrix = glm::lookAt(ViewPosition, ViewPosition + ForwardVector, UpVector);
        ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
        InverseViewMatrix = glm::inverse(ViewMatrix);
    }
}
