#pragma once

#include "Lumina.h"
#include <entt/entt.hpp>
#include "Components/NameComponent.h"
#include "Components/TransformComponent.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "World/World.h"


namespace Lumina
{
    class Entity
    {
    public:
        
        Entity() = default;
        Entity(const entt::entity& InHandle, CWorld* InWorld)
            : EntityHandle(InHandle)
            , World(InWorld)
        {
            Assert(EntityHandle != entt::null)
            Assert(World.IsValid())
        }

        LUMINA_API FArchive& Serialize(FArchive& Ar);

        LUMINA_API bool IsValid() const                    { return EntityHandle != entt::null && World.IsValid(); }
        LUMINA_API bool IsChild();
        LUMINA_API Entity GetParent();
        LUMINA_API CWorld* GetWorld() const                { return World; }
        LUMINA_API entt::entity GetHandle() const          { return EntityHandle; }
        LUMINA_API entt::entity GetHandleChecked() const   { Assert(EntityHandle != entt::null) return EntityHandle; }

        LUMINA_API const FName& GetName() const            { return GetConstComponent<SNameComponent>().Name; }
        LUMINA_API FTransform GetWorldTransform();
        LUMINA_API FTransform GetLocalTransform();
        
        LUMINA_API glm::mat4 GetWorldMatrix()       { return GetComponent<STransformComponent>().GetMatrix(); }
        LUMINA_API glm::vec3 GetLocation()          { return GetComponent<STransformComponent>().GetLocation(); }
        LUMINA_API glm::quat GetRotation()          { return GetComponent<STransformComponent>().GetRotation(); }
        LUMINA_API glm::vec3 GetScale()             { return GetComponent<STransformComponent>().GetScale(); }
        
        
        template <typename T, typename... Args>
        T& AddComponent(Args&&... args);

        template <typename T>
        bool HasComponent() const;
        
        template <typename T>
        T* TryGetComponent();

        template <typename T>
        T& GetComponent();

        template<typename T, typename... Func>
        T& Patch(Func&&... func);

        template<typename T>
        T& GetOrAddComponent();
        
        template <typename T>
        const T& GetConstComponent() const;

        template <typename T>
        SIZE_T RemoveComponent();
        
        operator entt::entity() const               { return EntityHandle; }
        operator uint32()                           { return static_cast<uint32>(EntityHandle); }
        operator bool() const                       { return EntityHandle != entt::null && World.IsValid(); }
        bool operator==(const Entity& other) const  { return EntityHandle == other.EntityHandle; }
        bool operator!=(const Entity& other) const  { return !(*this == other); }


        
    private:

        entt::entity EntityHandle;
        TObjectHandle<CWorld> World;
    };

    
    template <typename T, typename... Args>
    T& Entity::AddComponent(Args&&... args)
    {
        return World->GetMutableEntityRegistry().emplace_or_replace<T>(EntityHandle, std::forward<Args>(args)...);
    }
    
    template <typename T>
    bool Entity::HasComponent() const
    {
        return World->GetConstEntityRegistry().all_of<T>(EntityHandle);
    }
    
    template <typename T>
    T* Entity::TryGetComponent()
    {
        return World->GetMutableEntityRegistry().try_get<T>(EntityHandle);
    }

    template <typename T>
    T& Entity::GetComponent()
    {
        return World->GetMutableEntityRegistry().get<T>(EntityHandle);
    }

    template <typename T, typename ... Func>
    T& Entity::Patch(Func&&... func)
    {
        return World->GetMutableEntityRegistry().patch<T>(GetHandle(), std::forward<Func>(func)...);
    }

    template <typename T>
    T& Entity::GetOrAddComponent()
    {
        return World->GetMutableEntityRegistry().get_or_emplace<T>(EntityHandle);
    }

    template <typename T>
    const T& Entity::GetConstComponent() const
    {
        return World->GetConstEntityRegistry().get<T>(EntityHandle);
    }

    template <typename T>
    SIZE_T Entity::RemoveComponent()
    {
        return World->GetMutableEntityRegistry().remove<T>(EntityHandle);
    }
}
