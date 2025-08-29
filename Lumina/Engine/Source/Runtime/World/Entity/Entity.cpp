
#include "Entity.h"
#include "Components/RelationshipComponent.h"
#include "Core/Object/Class.h"

namespace Lumina
{
    FArchive& Entity::Serialize(FArchive& Ar)
    {
        return Ar;
    }
    
    bool Entity::IsChild()
    {
        if (auto* Component = TryGetComponent<SRelationshipComponent>())
        {
            return Component->Parent.IsValid();
        }

        return false;
    }

    Entity Entity::GetParent()
    {
        Assert(IsChild())

        return GetComponent<SRelationshipComponent>().Parent;
    }

    FTransform Entity::GetWorldTransform()
    {
        auto& TransformComp = GetComponent<STransformComponent>();
        return TransformComp.WorldTransform;
    }

    FTransform Entity::GetLocalTransform()
    {
        auto& TransformComp = GetComponent<STransformComponent>();
   
        if (IsChild())
        {
            return TransformComp.Transform;
        }
        else
        {
            return FTransform();
        }
    }
}
