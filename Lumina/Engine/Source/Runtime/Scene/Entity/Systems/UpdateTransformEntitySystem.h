#pragma once
#include "Core/Object/ObjectMacros.h"
#include "EntitySystem.h"
#include "UpdateTransformEntitySystem.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class LUMINA_API CUpdateTransformEntitySystem : public CEntitySystem
    {
        GENERATED_BODY()
    public:

        DEFINE_SCENE_SYSTEM(CUpdateTransformEntitySystem, RequiresUpdate(EUpdateStage::FrameStart, EUpdatePriority::Highest))
        
        void Initialize(const FSubsystemManager* SubsystemManager) override;
        void Shutdown() override;

        void Update(FEntityRegistry& EntityRegistry, const FSceneUpdateContext& UpdateContext) override;
        
    
    };
}
