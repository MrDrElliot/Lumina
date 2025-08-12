#include "UpdateTransformEntitySystem.h"

#include "Scene/Entity/Components/RelationshipComponent.h"
#include "scene/entity/components/transformcomponent.h"

namespace Lumina
{
    
    void CUpdateTransformEntitySystem::Initialize(const FSubsystemManager* SubsystemManager)
    {
        CEntitySystem::Initialize(SubsystemManager);
    }

    void CUpdateTransformEntitySystem::Shutdown()
    {
        CEntitySystem::Shutdown();
    }

    void CUpdateTransformEntitySystem::Update(FEntityRegistry& EntityRegistry, const FSceneUpdateContext& UpdateContext)
    {
        TFunction<void(const FTransform&, const SRelationshipComponent&)> UpdateChildrenRecursive;
    }
}
