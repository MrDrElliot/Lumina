#include "UpdateTransformEntitySystem.h"

#include "TaskSystem/TaskSystem.h"
#include "World/Entity/Components/RelationshipComponent.h"
#include "World/entity/components/transformcomponent.h"

namespace Lumina
{
    
    void CUpdateTransformEntitySystem::Initialize()
    {
        
    }

    void CUpdateTransformEntitySystem::Shutdown()
    {
        
    }

    void CUpdateTransformEntitySystem::Update(FEntityRegistry& EntityRegistry, const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();
        
        auto Group = EntityRegistry.group<>(entt::get<STransformComponent>);
        auto RelationshipGroup = EntityRegistry.group<>(entt::get<STransformComponent, SRelationshipComponent>);
        FTaskSystem::Get()->ParallelFor(Group.size(), [&](uint32 Index)
        {
            entt::entity entity = Group[Index];
            auto& transform = Group.get<STransformComponent>(entity);
            
            if (RelationshipGroup.contains(entity))
            {
                auto& relationship = RelationshipGroup.get<SRelationshipComponent>(entity);
            
                if (relationship.Parent.IsValid())
                {
                    transform.WorldTransform = relationship.Parent.GetComponent<STransformComponent>().WorldTransform * transform.Transform;
                }
                else
                {
                    transform.WorldTransform = transform.Transform;
                }
            }
            else
            {
                transform.WorldTransform = transform.Transform;
            }
            
            if (transform.CachedTransform != transform.Transform || RelationshipGroup.contains(entity))
            {
                transform.CachedMatrix    = transform.WorldTransform.GetMatrix();
                transform.CachedTransform = transform.Transform;
            }
        });

    }
}
