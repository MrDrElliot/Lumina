#pragma once
#include "Containers/Array.h"
#include "Core/Functional/Function.h"
#include "Core/Singleton/Singleton.h"
#include "entt/entt.hpp"

namespace Lumina
{
    using EntityComponentAddFn = TFunction<void(entt::entity, entt::registry&)>;

    
    class LUMINA_API FEntityComponentRegistry : public TSingleton<FEntityComponentRegistry>
    {
    public:

        struct FRegistration
        {
            const char* Name;
            EntityComponentAddFn Fn;
        };

        void RegisterComponent(const char* Name, EntityComponentAddFn Fn);
        EntityComponentAddFn GetComponentFn(const char* Name);

    private:
    
        TFixedVector<FRegistration, 200> Registrations;
    };
}

namespace Lumina
{
    #define REGISTER_ENTITY_COMPONENT(Type)                                      \
        struct AutoRegister_##Type {                                             \
            AutoRegister_##Type() {                                              \
                FEntityComponentRegistry::Get()->RegisterComponent(              \
                    #Type,                                                       \
                    [](entt::entity e, entt::registry& reg) {                    \
                        reg.emplace_or_replace<Type>(e);                         \
                    }                                                            \
                );                                                               \
            }                                                                    \
        };                                                                       \
        inline AutoRegister_##Type AutoRegisterInstance_##Type
}


