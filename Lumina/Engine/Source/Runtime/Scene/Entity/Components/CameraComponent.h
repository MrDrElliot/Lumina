#pragma once
#include "Component.h"
#include "Platform/WindowsPlatform.h"
#include "Renderer/ViewVolume.h"


namespace Lumina
{
    class FCameraComponent : public FEntityComponent
    {
    public:
        FCameraComponent(float fov = 90.0f, float aspect = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 1000.0f)
            :ViewVolume(fov, aspect, nearPlane, farPlane)
        {}

        FORCEINLINE void SetView(const glm::vec3& Position, const glm::vec3& ViewDirection, const glm::vec3& UpDirection = glm::vec3(0, 1, 0))
        {
            ViewVolume.SetView(Position, ViewDirection, UpDirection);
        }
        
        FORCEINLINE void SetFOV(float NewFOV)
        {
            ViewVolume.SetPerspective(NewFOV, ViewVolume.GetAspectRatio(), ViewVolume.GetNearPlane(), ViewVolume.GetFarPlane());
        }
        
        FORCEINLINE void SetAspectRatio(float NewAspect)
        {
            ViewVolume.SetPerspective(ViewVolume.GetFOV(), NewAspect, ViewVolume.GetNearPlane(), ViewVolume.GetFarPlane());
        }
        
        FORCEINLINE void SetDepthRange(float NearPlane, float FarPlane)
        {
            ViewVolume.SetDepthRange(glm::vec2(NearPlane, FarPlane));
        }

        FORCEINLINE void SetPosition(const glm::vec3& NewPosition)
        {
            ViewVolume.SetViewPosition(NewPosition);
        }

        FORCEINLINE float GetFOV() const { return ViewVolume.GetFOV(); }
        FORCEINLINE float GetAspectRatio() const { return ViewVolume.GetAspectRatio(); }
        FORCEINLINE const glm::vec3& GetPosition() const { return ViewVolume.GetViewPosition(); }
        FORCEINLINE const glm::mat4& GetViewMatrix() const { return ViewVolume.GetViewMatrix(); }
        FORCEINLINE const glm::mat4& GetProjectionMatrix() const { return ViewVolume.GetProjectionMatrix(); }
        FORCEINLINE const glm::mat4& GetViewProjectionMatrix() const { return ViewVolume.GetViewProjectionMatrix(); }

    private:
        
        FViewVolume ViewVolume;
    };
    
}
