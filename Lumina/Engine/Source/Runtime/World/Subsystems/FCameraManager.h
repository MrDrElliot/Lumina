#pragma once
#include "World/Entity/Entity.h"


namespace Lumina
{
    class FCameraManager
    {
    public:

        FCameraManager() = default;
        
        void SetActiveCamera(const Entity& NewCameraEntity)
        {
            ActiveCameraEntity = NewCameraEntity;
            CameraComponent = &ActiveCameraEntity.GetComponent<SCameraComponent>();
        }
        
        Entity GetActiveCameraEntity() const { return ActiveCameraEntity; }
        SCameraComponent* GetCameraComponent() const { return CameraComponent; }
    

    private:

        SCameraComponent* CameraComponent = nullptr;
        Entity ActiveCameraEntity;
    
    };
}
