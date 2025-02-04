#pragma once

#include "entt/entt.hpp"
#include "Containers/String.h"
#include "Scene/SceneCommon.h"
#include "Scene/Components/NameComponent.h"
#include "Scene/Components/TransformComponent.h"


namespace Lumina
{
    class Entity
    {
    public:
        
        Entity() = default;
        Entity(const entt::entity& InHandle, FScene* InScene);
        Entity(const entt::entity& InHandle, TSharedPtr<FScene> InScene);

        
        FScene* GetScene()                      { return mScene; }
        entt::entity GetHandle()                { return mEntityHandle; }

        inline FString& GetName()               { return GetComponent<FNameComponent>().GetName(); }
        inline FTransform GetTransform()        { return GetComponent<FTransformComponent>().GetTransform(); }
        inline glm::vec3 GetLocation()          { return GetComponent<FTransformComponent>().GetLocation(); }
        inline glm::quat GetRotation()          { return GetComponent<FTransformComponent>().GetRotation(); }
        inline glm::vec3 GetScale()             { return GetComponent<FTransformComponent>().GetScale(); }

        inline void Destroy()                   { if (mScene) mScene->DestroyEntity(*this);  }
        
        template <typename T, typename... Args>
        auto AddComponent(Args&&... args) -> decltype(auto);

        template <typename T>
        bool HasComponent();

        template <typename T>
        T* TryGetComponent();

        template <typename T>
        T& GetComponent();


        

        inline operator entt::entity()              { return mEntityHandle; }
        operator uint32()                           { return (uint32)mEntityHandle; }
        operator bool() const                       { return mEntityHandle != entt::null && mScene; }
        bool operator==(const Entity& other) const  { return mEntityHandle == other.mEntityHandle; }
        bool operator!=(const Entity& other) const  { return !(*this == other); }


        
    private:

        entt::entity mEntityHandle;
        FScene* mScene = nullptr;
    
    };

    
    template <typename T, typename... Args>
    auto Entity::AddComponent(Args&&... args) -> decltype(auto)
    {
        return mScene->GetEntityRegistry().emplace_or_replace<T>(mEntityHandle, std::forward<Args>(args)...);
    }
    
    template <typename T>
    bool Entity::HasComponent()
    {
        if(Entity::mScene)
        {
            return mScene->GetEntityRegistry().all_of<T>(mEntityHandle);
        }
        
        return false;
    }

    template <typename T>
    T* Entity::TryGetComponent()
    {
        return mScene->GetEntityRegistry().try_get<T>(mEntityHandle);
    }

    template <typename T>
    T& Entity::GetComponent()
    {
        return mScene->GetEntityRegistry().get<T>(mEntityHandle);
    }
}
