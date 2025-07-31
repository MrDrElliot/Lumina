#pragma once
#include "EntitySystem.h"
#include "glm/glm.hpp"
#include "DebugCameraEntitySystem.generated.h"


namespace Lumina
{
    LUM_CLASS()
    class LUMINA_API CDebugCameraEntitySystem : public CEntitySystem
    {
        GENERATED_BODY()
    public:

        DEFINE_SCENE_SYSTEM(CDebugCameraEntitySystem, RequiresUpdate(EUpdateStage::FrameStart, EUpdatePriority::Highest))
        
        void Initialize(const FSubsystemManager* SubsystemManager) override;
        void Shutdown() override;

        void Update(FEntityRegistry& EntityRegistry, const FSceneUpdateContext& UpdateContext) override;

    private:

    };
}
