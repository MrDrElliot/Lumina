#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Platform/WindowsPlatform.h"

namespace Lumina
{
    class FViewVolume
    {
    public:

        FViewVolume(float fov = 90.0f, float aspect = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 1000.0f);

        void SetViewPosition(const glm::vec3& Position);
        void SetView(const glm::vec3& Position, const glm::vec3& ViewDirection, const glm::vec3& UpDirection);
        void SetPerspective(float InFov, float InAspect, float InNearPlane, float InFarPlane);
        void SetAspectRatio(float InAspect);
        void SetDepthRange(const glm::vec2& InRange);
        void SetFOV(float InFOV);
    
        FORCEINLINE const glm::vec3& GetViewPosition() const { return ViewPosition; }

        FORCEINLINE const glm::mat4& GetViewMatrix() const { return ViewMatrix; }
        FORCEINLINE const glm::mat4& GetViewProjectionMatrix() const { return ViewProjectionMatrix; }
        FORCEINLINE const glm::mat4& GetProjectionMatrix() const { return ProjectionMatrix; }
        FORCEINLINE const glm::mat4& GetInverseProjectionMatrix() const { return InverseProjectionMatrix; }

        FORCEINLINE float GetFOV() const { return FOV; }
        FORCEINLINE float GetAspectRatio() const { return AspectRatio; }
        FORCEINLINE float GetNearPlane() const { return DepthRange.x; }
        FORCEINLINE float GetFarPlane() const { return DepthRange.y; }
        
    private:

        void UpdateMatrices();
    
        glm::vec3           ViewPosition;
        glm::vec3           ForwardVector;
        glm::vec3           UpVector;
        glm::vec3           RightVector;

        glm::mat4           ProjectionMatrix;
        glm::mat4           InverseProjectionMatrix;
        glm::mat4           ViewMatrix;
        glm::mat4           ViewProjectionMatrix;

        float               FOV = 90.0f;
        float               AspectRatio = 16.0f/9.0f;
        glm::vec2           DepthRange = {0.1f, 1000.0f};
    };

}
