#pragma once

#include "Lumina.h"
#include <entt/entt.hpp>

#include "Components/NameComponent.h"
#include "Components/TransformComponent.h"
#include "Scene/Scene.h"


namespace Lumina
{
    class Entity
    {
    public:
        
        Entity() = default;
        Entity(const entt::entity& InHandle, FScene* InScene)
            : EntityHandle(InHandle)
            , Scene(InScene)
        {
            Assert(EntityHandle != entt::null);
            Assert(Scene != nullptr);
        }

        
        FORCEINLINE FScene* GetScene() const                { return Scene; }
        FORCEINLINE entt::entity GetHandle() const          { return EntityHandle; }
        FORCEINLINE entt::entity GetHandleChecked() const   { Assert(EntityHandle != entt::null); return EntityHandle; }

        FORCEINLINE const FName& GetName() const     { return GetConstComponent<FNameComponent>().GetName(); }
        FORCEINLINE FTransform GetTransform()        { return GetComponent<FTransformComponent>().GetTransform(); }
        FORCEINLINE glm::vec3 GetLocation()          { return GetComponent<FTransformComponent>().GetLocation(); }
        FORCEINLINE glm::quat GetRotation()          { return GetComponent<FTransformComponent>().GetRotation(); }
        FORCEINLINE glm::vec3 GetScale()             { return GetComponent<FTransformComponent>().GetScale(); }

        FORCEINLINE void Destroy() const             { Assert(Scene != nullptr); Scene->DestroyEntity(*this);  }



        
        template <typename T, typename... Args>
        auto AddComponent(Args&&... args) -> decltype(auto);

        template <typename T>
        bool HasComponent() const;

        template <typename T>
        T* TryGetComponent();

        template <typename T>
        T& GetComponent();

        template <typename T>
        const T& GetConstComponent() const;

        
        inline operator entt::entity() const               { return EntityHandle; }
        inline operator uint32()                           { return static_cast<uint32>(EntityHandle); }
        inline operator bool() const                       { return EntityHandle != entt::null && Scene; }
        inline bool operator==(const Entity& other) const  { return EntityHandle == other.EntityHandle; }
        inline bool operator!=(const Entity& other) const  { return !(*this == other); }


        
    private:

        entt::entity EntityHandle;
        FScene* Scene = nullptr;
    
    };

    
    template <typename T, typename... Args>
    auto Entity::AddComponent(Args&&... args) -> decltype(auto)
    {
        return Scene->GetMutableEntityRegistry().emplace_or_replace<T>(EntityHandle, std::forward<Args>(args)...);
    }
    
    template <typename T>
    bool Entity::HasComponent() const
    {
        if(Entity::Scene)
        {
            return Scene->GetMutableEntityRegistry().all_of<T>(EntityHandle);
        }
        
        return false;
    }

    template <typename T>
    T* Entity::TryGetComponent()
    {
        return Scene->GetMutableEntityRegistry().try_get<T>(EntityHandle);
    }

    template <typename T>
    T& Entity::GetComponent()
    {
        return Scene->GetMutableEntityRegistry().get<T>(EntityHandle);
    }

    template <typename T>
    const T& Entity::GetConstComponent() const
    {
        return Scene->GetMutableEntityRegistry().get<T>(EntityHandle);
    }
}
