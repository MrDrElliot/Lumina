#pragma once

#include "Core/DisableAllWarnings.h"
#include "Core/Math/Frustum.h"
#include "Core/Object/ObjectMacros.h"
#include "Module/API.h"
PRAGMA_DISABLE_ALL_WARNINGS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
PRAGMA_ENABLE_ALL_WARNINGS
#include "Platform/WindowsPlatform.h"

namespace Lumina
{
    LUM_STRUCT()
    class LUMINA_API FViewVolume
    {
    public:

        FViewVolume(float fov = 90.0f, float aspect = 16.0f / 9.0f);

        void SetViewPosition(const glm::vec3& Position);
        void SetView(const glm::vec3& Position, const glm::vec3& ViewDirection, const glm::vec3& UpDirection);
        void SetPerspective(float InFov, float InAspect);
        void SetAspectRatio(float InAspect);
        void SetFOV(float InFOV);
    
        const glm::vec3& GetViewPosition() const { return ViewPosition; }

        const glm::mat4& GetViewMatrix() const { return ViewMatrix; }
        const glm::mat4& GetInverseViewMatrix() const { return InverseViewMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return ViewProjectionMatrix; }
        const glm::mat4& GetProjectionMatrix() const { return ProjectionMatrix; }
        const glm::mat4& GetInverseProjectionMatrix() const { return InverseProjectionMatrix; }
        const glm::vec3& GetForwardVector() const { return ForwardVector; }
        const glm::vec3& GetRightVector() const { return RightVector; }

        FFrustum GetFrustum() const;
        float GetFOV() const { return FOV; }
        float GetAspectRatio() const { return AspectRatio; }

        static glm::vec3 UpAxis;
        static glm::vec3 RightAxis;
        static glm::vec3 ForwardAxis;
        
    private:

        void UpdateMatrices();

        glm::vec3           ViewPosition;
        glm::vec3           ForwardVector;
        glm::vec3           UpVector;
        glm::vec3           RightVector;

        glm::mat4           ProjectionMatrix;
        glm::mat4           InverseProjectionMatrix;
        glm::mat4           ViewMatrix;
        glm::mat4           InverseViewMatrix;
        glm::mat4           ViewProjectionMatrix;
        
        float               FOV = 90.0f;
        float               AspectRatio = 16.0f/9.0f;
    };

}
