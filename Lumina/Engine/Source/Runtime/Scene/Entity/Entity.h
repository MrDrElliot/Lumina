#pragma once

#include "Lumina.h"
#include <entt/entt.hpp>

#include "Components/NameComponent.h"
#include "Components/TransformComponent.h"
#include "Scene/Scene.h"


namespace Lumina
{

    template <typename T, typename MemberPtr, typename Value>
    constexpr bool is_member_assignable_v = requires(T& t, MemberPtr member, Value&& val)
    {
        { t.*member = std::forward<Value>(val) };
    };
    
    
    class Entity
    {
    public:
        
        Entity() = default;
        Entity(const entt::entity& InHandle, FScene* InScene)
            : EntityHandle(InHandle)
            , Scene(InScene)
        {
            Assert(EntityHandle != entt::null)
            Assert(Scene != nullptr)
        }

        bool IsValid() const                                { return EntityHandle != entt::null && Scene != nullptr; }
        FORCEINLINE FScene* GetScene() const                { return Scene; }
        FORCEINLINE entt::entity GetHandle() const          { return EntityHandle; }
        FORCEINLINE entt::entity GetHandleChecked() const   { Assert(EntityHandle != entt::null) return EntityHandle; }

        FORCEINLINE const FName& GetName() const     { return GetConstComponent<SNameComponent>().Name; }
        FORCEINLINE FTransform GetTransform()        { return GetComponent<STransformComponent>().GetTransform(); }
        FORCEINLINE glm::vec3 GetLocation()          { return GetComponent<STransformComponent>().GetLocation(); }
        FORCEINLINE glm::quat GetRotation()          { return GetComponent<STransformComponent>().GetRotation(); }
        FORCEINLINE glm::vec3 GetScale()             { return GetComponent<STransformComponent>().GetScale(); }

        FORCEINLINE void Destroy() const             { Assert(Scene != nullptr) Scene->DestroyEntity(*this);  }

        
        
        template <typename T, typename... Args>
        T& AddComponent(Args&&... args);

        template <typename T>
        bool HasComponent() const;

        template <typename T, typename... Args>
        T& SetComponentProperty(Args&&... args);

        template <typename T>
        T& TryGetComponent();

        template <typename T>
        T& GetComponent();

        template<typename T, typename... Func>
        T& Patch(Func&&... func);

        template<typename T>
        T& AddOrReplace();

        template<typename T>
        T& GetOrAddComponent();
        
        template <typename T>
        const T& GetConstComponent() const;

        template <typename T>
        SIZE_T RemoveComponent() const;
        
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
    T& Entity::AddComponent(Args&&... args)
    {
        return Scene->GetMutableEntityRegistry().emplace_or_replace<T>(EntityHandle, std::forward<Args>(args)...);
    }
    
    template <typename T>
    bool Entity::HasComponent() const
    {
        return Scene->GetMutableEntityRegistry().all_of<T>(EntityHandle);
    }

    template <typename T, typename... Args>
    T& Entity::SetComponentProperty(Args&&... args)
    {
        static_assert(sizeof...(args) % 2 == 0, "Arguments must be (member, value) pairs");

        using TupleType = std::tuple<std::decay_t<Args>...>;
        constexpr size_t N = sizeof...(Args) / 2;

        [&]<std::size_t... I>(std::index_sequence<I...>)
        {
            static_assert((is_member_assignable_v<T, std::tuple_element_t<I * 2, TupleType>, std::tuple_element_t<I * 2 + 1, TupleType>> && ...),
                "At least one (member, value) pair is not assignable");
            
        }(std::make_index_sequence<N>{});

        auto& registry = Scene->GetMutableEntityRegistry();
        registry.patch<T>(EntityHandle, [args_tuple = std::make_tuple(std::forward<Args>(args)...)](T& component)
        {
            [&]<std::size_t... I>(std::index_sequence<I...>)
            {
                (..., (
                    component.*std::get<I * 2>(args_tuple) = std::get<I * 2 + 1>(args_tuple)
                ));
            }(std::make_index_sequence<N>{});
        });

        return registry.get<T>(EntityHandle);
    }
    
    template <typename T>
    T& Entity::TryGetComponent()
    {
        return Scene->GetMutableEntityRegistry().try_get<T>(EntityHandle);
    }

    template <typename T>
    T& Entity::GetComponent()
    {
        return Scene->GetMutableEntityRegistry().get<T>(EntityHandle);
    }

    template <typename T, typename ... Func>
    T& Entity::Patch(Func&&... func)
    {
        return Scene->GetMutableEntityRegistry().patch<T>(GetHandle(), std::forward<Func>(func)...);
    }

    template <typename T>
    T& Entity::AddOrReplace()
    {
        return Scene->GetMutableEntityRegistry().emplace_or_replace<T>(EntityHandle);
    }

    template <typename T>
    T& Entity::GetOrAddComponent()
    {
        if (HasComponent<T>())
        {
            return GetComponent<T>();
        }

        return AddComponent<T>();
    }

    template <typename T>
    const T& Entity::GetConstComponent() const
    {
        return Scene->GetMutableEntityRegistry().get<T>(EntityHandle);
    }

    template <typename T>
    SIZE_T Entity::RemoveComponent() const
    {
        return Scene->GetMutableEntityRegistry().remove<T>(EntityHandle);
    }
}
