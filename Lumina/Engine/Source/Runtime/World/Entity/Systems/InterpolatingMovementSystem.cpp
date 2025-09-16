#include "InterpolatingMovementSystem.h"

#include "World/Entity/Components/InterpolatingMovementComponent.h"
#include "World/Entity/Components/TransformComponent.h"

namespace Lumina
{
    void CInterpolatingMovementSystem::Update(FSystemContext& SystemContext)
    {
        LUMINA_PROFILE_SCOPE();

        
        auto View = SystemContext.CreateView<SInterpolatingMovementComponent, STransformComponent>();
        View.each([&](SInterpolatingMovementComponent& MoveComp, STransformComponent& TransformComp)
        {
            float DeltaAlpha = 1.0f * SystemContext.GetDeltaTime();

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
