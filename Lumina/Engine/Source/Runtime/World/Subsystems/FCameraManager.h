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
        }
        
        Entity GetActiveCameraEntity() const { return ActiveCameraEntity; }
    

    private:

        Entity ActiveCameraEntity;
    
    };
}
