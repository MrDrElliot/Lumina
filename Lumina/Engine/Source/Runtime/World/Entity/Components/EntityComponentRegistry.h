#pragma once
#include "Containers/Array.h"
#include "Core/Functional/Function.h"
#include "Core/Singleton/Singleton.h"
#include "entt/entt.hpp"
#include "World/Entity/Registry/EntityRegistry.h"

namespace Lumina
{
    struct SEntityComponent;
    class FEntityRegistry;

    using EntityComponentAddFn = SEntityComponent* (*)(entt::entity, FEntityRegistry&);
    
    class LUMINA_API FEntityComponentRegistry : public TSingleton<FEntityComponentRegistry>
    {
    public:

        struct FRegistration
        {
            const char* Name;
            EntityComponentAddFn Fn;
        };

        void RegisterComponent(const char* Name, EntityComponentAddFn Fn)
        {
            Registrations.push_back({ Name, Fn });
        }

        EntityComponentAddFn GetComponentFn(const char* Name)
        {
            for (auto& reg : Registrations)
            {
                if (strcmp(reg.Name, Name) == 0)
                    return reg.Fn;
            }
            return nullptr;
        }

    private:
        TFixedVector<FRegistration, 200> Registrations;
    };
}


namespace Lumina
{
    #define REGISTER_ENTITY_COMPONENT(Type)                                      \
        namespace {                                                              \
            SEntityComponent* AddComponent_##Type(entt::entity e, FEntityRegistry& reg) \
            {                                                                     \
                return &reg.emplace_or_replace<Type>(e);                          \
            }                                                                     \
        }                                                                         \
        struct AutoRegister_##Type {                                              \
            AutoRegister_##Type() {                                               \
                FEntityComponentRegistry::Get().RegisterComponent(#Type, AddComponent_##Type); \
            }                                                                     \
        };                                                                        \
        inline AutoRegister_##Type AutoRegisterInstance_##Type
}

