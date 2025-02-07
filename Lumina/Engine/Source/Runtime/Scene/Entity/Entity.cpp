
#include "Entity.h"
#include "Scene/Scene.h"

namespace Lumina
{

    Entity::Entity(const entt::entity& InHandle, TRefPtr<FScene> InScene)
    :mEntityHandle(InHandle), mScene(InScene)
    {
    }
    
}
