#pragma once

#include "entt/entt.hpp"
#include "Containers/String.h"
#include "Scene/SceneCommon.h"
#include "Scene/Components/NameComponent.h"
#include "Scene/Components/TransformComponent.h"


namespace Lumina
{
    class LScene;

    class Entity
    {
    public:
        
        Entity() = default; // Invalid entity.
        Entity(const entt::entity& InHandle, LScene* InScene);
        Entity(const entt::entity& InHandle, std::shared_ptr<LScene> InScene);

        
        LScene* GetScene()                      { return mScene; }
        entt::entity GetHandle()                { return mEntityHandle; }

        inline LString& GetName()                { return GetComponent<FNameComponent>().GetName(); }
        inline FTransform GetTransform()        { return GetComponent<FTransformComponent>().GetTransform(); }
        inline glm::vec3 GetLocation()          { return GetComponent<FTransformComponent>().GetLocation(); }
        inline glm::quat GetRotation()          { return GetComponent<FTransformComponent>().GetRotation(); }
        inline glm::vec3 GetScale()             { return GetComponent<FTransformComponent>().GetScale(); }

        inline void Destroy()                   { if (mScene) mScene->DestroyEntity(*this);  }
        
        template <typename T, typename... Args>
        void AddComponent(Args&&... args);

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
        LScene* mScene = nullptr;
    
    };

    
    template <typename T, typename... Args>
    void Entity::AddComponent(Args&&... args)
    {
        mScene->GetEntityRegistry().emplace_or_replace<T>(mEntityHandle, std::forward<Args>(args)...);
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
