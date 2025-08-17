
#include "Entity.h"
#include "Components/RelationshipComponent.h"
#include "Core/Object/Class.h"

namespace Lumina
{
    FArchive& Entity::Serialize(FArchive& Ar)
    {
        if (Ar.IsWriting())
        {
            Assert(World.IsValid())
            
            SIZE_T NumComponents = 0;
            TVector<TPair<FName, TPair<CStruct*, SEntityComponent*>>> Components;
    
            for (auto [ID, Set] : World->GetMutableEntityRegistry().storage())
            {
                if (Set.contains(GetHandle()))
                {
                    if (void* ComponentPtr = Set.value(GetHandle()))
                    {
                        auto* EntityComponent = static_cast<SEntityComponent*>(ComponentPtr);
                        
                        if (CStruct* Type = EntityComponent->GetType())
                        {
                            Components.emplace_back(
                                Type->GetName(),
                                eastl::make_pair(Type, EntityComponent)
                            );
                        }
                    }
                }
            }
    
            NumComponents = Components.size();
            Ar << NumComponents;
    
            for (auto& [TypeName, Type] : Components)
            {
                Ar << TypeName;
                Type.first->SerializeTaggedProperties(Ar, Type.second);
            }
    
            if (World->GetMutableEntityRegistry().all_of<SRelationshipComponent>(GetHandle()))
            {
                auto& rel = World->GetMutableEntityRegistry().get<SRelationshipComponent>(GetHandle());
                SIZE_T NumChildren = rel.Size;
                Ar << NumChildren;

                for (SIZE_T i = 0; i < NumChildren; ++i)
                {
                    rel.Children[i].Serialize(Ar);
                }
            }
            else
            {
                SIZE_T NumChildren = 0;
                Ar << NumChildren;
            }
        }
        else if (Ar.IsReading())
        {
            SIZE_T NumComponents = 0;
            Ar << NumComponents;
    
            for (SIZE_T i = 0; i < NumComponents; ++i)
            {
                FName TypeName;
                Ar << TypeName;
    
                if (CStruct* Struct = FindObject<CStruct>("script://lumina", TypeName))
                {
                    auto Fn = FEntityComponentRegistry::Get()->GetComponentFn(Struct->GetName().c_str());
                    SEntityComponent* NewComponent = Fn(GetHandle(), World->GetMutableEntityRegistry());
                    Struct->SerializeTaggedProperties(Ar, NewComponent);
                }
                else
                {
                    
                }
            }
    
            SIZE_T NumChildren = 0;
            Ar << NumChildren;
    
            if (NumChildren > 0)
            {
                auto& rel = World->GetMutableEntityRegistry().emplace<SRelationshipComponent>(GetHandle());
    
                for (SIZE_T i = 0; i < NumChildren; ++i)
                {
                    // Create new child entity
                    entt::entity ChildHandle = World->GetMutableEntityRegistry().create();
                    rel.Children[rel.Size] = Entity(ChildHandle, World);
                    rel.Size++;
    
                    Entity ChildEntity(ChildHandle, World);
                    ChildEntity.Serialize(Ar);
    
                    // Patch parent relationship
                    if (World->GetMutableEntityRegistry().all_of<SRelationshipComponent>(ChildHandle))
                    {
                        auto& childRel = World->GetMutableEntityRegistry().get<SRelationshipComponent>(ChildHandle);
                        childRel.Parent = *this;
                    }
                    else
                    {
                        auto& childRel = World->GetMutableEntityRegistry().emplace<SRelationshipComponent>(ChildHandle);
                        childRel.Parent = *this;
                    }
                }
            }
        }
    
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
