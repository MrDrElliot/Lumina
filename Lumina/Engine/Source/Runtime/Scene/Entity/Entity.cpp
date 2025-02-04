
#include "Entity.h"
#include "Scene/Scene.h"

namespace Lumina
{
    Entity::Entity(const entt::entity& InHandle, FScene* InScene)
    :mEntityHandle(InHandle), mScene(InScene)
    {
                
    }

    Entity::Entity(const entt::entity& InHandle, TSharedPtr<FScene> InScene)
    :mEntityHandle(InHandle), mScene(InScene.get())
    {
    }
    
}
