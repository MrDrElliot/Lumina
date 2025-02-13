#pragma once
#include "EntitySystem.h"
#include "glm/glm.hpp"

namespace Lumina
{
    class FCameraComponent;
}

namespace Lumina
{
    class FDebugCameraEntitySystem : public FEntitySystem
    {
    public:

        DEFINE_SCENE_SYSTEM(FDebugCameraEntitySystem, RequiresUpdate(EUpdateStage::FrameStart, EUpdatePriority::Highest))
        
        void Initialize(const FSubsystemManager* SubsystemManager) override;
        void Shutdown() override;

        void Update(FEntityRegistry& EntityRegistry, const FSceneUpdateContext& UpdateContext) override;

    private:
        
        glm::vec2 PreviousMousePos = glm::vec2(0.0f);
        bool bFirstMove = true;
    };
}
