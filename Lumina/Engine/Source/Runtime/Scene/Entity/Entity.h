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

        Entity(const entt::entity& InHandle, LScene* InScene);
        Entity(const entt::entity& InHandle, std::shared_ptr<LScene> InScene);

        
        LScene* GetScene()                      { return mScene; }
        inline LString GetName()                { return GetComponent<NameComponent>().GetName(); }
        entt::entity GetHandle()                { return mEntityHandle; }
        inline FTransform GetTransform()        { return GetComponent<TransformComponent>().Transform; }
        inline glm::vec3 GetLocation()          { return GetComponent<TransformComponent>().Transform.Location; }
        inline glm::quat GetRotation()          { return GetComponent<TransformComponent>().Transform.Rotation; }
        inline glm::vec3 GetScale()             { return GetComponent<TransformComponent>().Transform.Scale; }

        
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
