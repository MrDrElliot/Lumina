#include "InterpolatingMovementSystem.h"

#include "Scene/SceneUpdateContext.h"
#include "Scene/Entity/Components/InterpolatingMovementComponent.h"
#include "Scene/Entity/Components/TransformComponent.h"
#include "Scene/Entity/Registry/EntityRegistry.h"

namespace Lumina
{
    void CInterpolatingMovementSystem::Update(FEntityRegistry& EntityRegistry, const FSceneUpdateContext& UpdateContext)
    {
        auto Group = EntityRegistry.group<SInterpolatingMovementComponent, STransformComponent>();
        Group.each([&](SInterpolatingMovementComponent& MoveComp, STransformComponent& TransformComp)
        {
            float DeltaAlpha = 1.0f * UpdateContext.GetDeltaTime();

            if (MoveComp.bForward)
            {
                MoveComp.Alpha += (MoveComp.Speed * DeltaAlpha);
                if (MoveComp.Alpha >= 1.0f)
                {
                    MoveComp.Alpha = 1.0f;
                    MoveComp.bForward = false;
                }
            }
            else
            {
                MoveComp.Alpha -= (MoveComp.Speed * DeltaAlpha);
                if (MoveComp.Alpha <= 0.0f)
                {
                    MoveComp.Alpha = 0.0f;
                    MoveComp.bForward = true;
                }
            }

            TransformComp.SetLocation(glm::mix(MoveComp.Start.Location, MoveComp.End.Location, MoveComp.Alpha));
            TransformComp.SetRotation(glm::slerp(MoveComp.Start.Rotation, MoveComp.End.Rotation, MoveComp.Alpha));
            TransformComp.SetScale(glm::mix(MoveComp.Start.Scale, MoveComp.End.Scale, MoveComp.Alpha));
        });
    }
}
