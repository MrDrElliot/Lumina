#include "Entity.h"

#include "Scene/Scene.h"

namespace Lumina
{
    Entity::Entity(const entt::entity& InHandle, LScene* InScene)
    :mEntityHandle(InHandle), mScene(InScene)
    {
                
    }

    Entity::Entity(const entt::entity& InHandle, std::shared_ptr<LScene> InScene)
    :mEntityHandle(InHandle), mScene(InScene.get())
    {
    }
    
}
