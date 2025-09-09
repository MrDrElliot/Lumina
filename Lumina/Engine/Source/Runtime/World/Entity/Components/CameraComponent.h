#pragma once
#include "Component.h"
#include "Renderer/ViewVolume.h"
#include "CameraComponent.generated.h"


namespace Lumina
{
    LUM_PROPERTY()
    struct LUMINA_API SCameraComponent
    {
        GENERATED_BODY()
        ENTITY_COMPONENT(SCameraComponent)
        
        SCameraComponent(float fov = 90.0f, float aspect = 16.0f / 9.0f)
            :ViewVolume(fov, aspect)
        {}

        void SetView(const glm::vec3& Position, const glm::vec3& ViewDirection, const glm::vec3& UpDirection = glm::vec3(0, 1, 0))
        {
            ViewVolume.SetView(Position, ViewDirection, UpDirection);
        }
        
        void SetFOV(float NewFOV)
        {
            ViewVolume.SetPerspective(NewFOV, ViewVolume.GetAspectRatio());
        }
        
        void SetAspectRatio(float NewAspect)
        {
            ViewVolume.SetPerspective(ViewVolume.GetFOV(), NewAspect);
        }

        void SetPosition(const glm::vec3& NewPosition)
        {
            ViewVolume.SetViewPosition(NewPosition);
        }

        float GetFOV() const { return ViewVolume.GetFOV(); }
        float GetAspectRatio() const { return ViewVolume.GetAspectRatio(); }
        const glm::vec3& GetPosition() const { return ViewVolume.GetViewPosition(); }
        const glm::mat4& GetViewMatrix() const { return ViewVolume.GetViewMatrix(); }
        const glm::mat4& GetProjectionMatrix() const { return ViewVolume.GetProjectionMatrix(); }
        const glm::mat4& GetViewProjectionMatrix() const { return ViewVolume.GetViewProjectionMatrix(); }
        const FViewVolume& GetViewVolume() const { return ViewVolume; }
        const glm::vec3& GetForwardVector() const { return ViewVolume.GetForwardVector(); }
        const glm::vec3& GetRightVector() const { return ViewVolume.GetRightVector(); }
        
    private:
        
        FViewVolume ViewVolume;
    };
}
