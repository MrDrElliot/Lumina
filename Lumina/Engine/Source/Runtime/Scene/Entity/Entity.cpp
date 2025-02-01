
#include "Entity.h"
#include "Scene/Scene.h"

namespace Lumina
{
    Entity::Entity(const entt::entity& InHandle, AScene* InScene)
    :mEntityHandle(InHandle), mScene(InScene)
    {
                
    }

    Entity::Entity(const entt::entity& InHandle, TSharedPtr<AScene> InScene)
    :mEntityHandle(InHandle), mScene(InScene.get())
    {
    }
    
}
