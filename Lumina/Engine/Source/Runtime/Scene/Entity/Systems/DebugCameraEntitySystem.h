#pragma once
#include "EntitySystem.h"
#include "glm/glm.hpp"


namespace Lumina
{
    class LUMINA_API FDebugCameraEntitySystem : public FEntitySystem
    {
    public:

        DEFINE_SCENE_SYSTEM(FDebugCameraEntitySystem, RequiresUpdate(EUpdateStage::FrameStart, EUpdatePriority::Highest))
        
        void Initialize(const FSubsystemManager* SubsystemManager) override;
        void Shutdown() override;

        void Update(FEntityRegistry& EntityRegistry, const FSceneUpdateContext& UpdateContext) override;

    private:

    };
}
