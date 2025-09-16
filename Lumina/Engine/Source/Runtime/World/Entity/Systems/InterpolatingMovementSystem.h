#pragma once
#include "EntitySystem.h"
#include "Core/Object/ObjectMacros.h"
#include "InterpolatingMovementSystem.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class LUMINA_API CInterpolatingMovementSystem : public CEntitySystem
    {
        GENERATED_BODY()
        DEFINE_SCENE_SYSTEM(CInterpolatingMovementSystem, RequiresUpdate(EUpdateStage::PrePhysics, EUpdatePriority::Default))

    public:

        void Update(FSystemContext& SystemContext) override;
    
    };
}
