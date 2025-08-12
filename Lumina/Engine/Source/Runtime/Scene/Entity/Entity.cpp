
#include "Entity.h"

#include "Components/RelationshipComponent.h"

namespace Lumina
{
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
    
        if (IsChild())
        {
            auto& RelationshipComp = GetComponent<SRelationshipComponent>();
            FTransform ParentWorldTransform = RelationshipComp.Parent.GetWorldTransform();
            return ParentWorldTransform * TransformComp.Transform;
        }
        else
        {
            return TransformComp.Transform;
        }
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
