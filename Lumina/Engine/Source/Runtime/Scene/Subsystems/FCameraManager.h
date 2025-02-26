#pragma once
#include <entt/entt.hpp>
#include "Memory/RefCounted.h"
#include "Scene/Entity/Entity.h"
#include "Subsystems/Subsystem.h"


namespace Lumina
{
    class FScene;

    class FCameraManager : public ISubsystem
    {
    public:

        FCameraManager() = default;

        void Initialize(FSubsystemManager& Manager) override {}
        void Deinitialize() override {}


        FORCEINLINE void SetActiveCamera(const Entity& NewCameraEntity)
        {
            ActiveCameraEntity = NewCameraEntity;
        }
        
        FORCEINLINE Entity GetActiveCameraEntity() const { return ActiveCameraEntity; }
    

    private:

        Entity              ActiveCameraEntity;
    
    };
}
