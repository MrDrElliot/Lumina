#include "InterpolatingMovementSystem.h"

#include "World/Entity/Components/InterpolatingMovementComponent.h"
#include "World/Entity/Components/TransformComponent.h"
#include "World/Entity/Registry/EntityRegistry.h"

namespace Lumina
{
    void CInterpolatingMovementSystem::Update(FEntityRegistry& EntityRegistry, const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();

        auto Group = EntityRegistry.group<SInterpolatingMovementComponent>(entt::get<STransformComponent>);
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
