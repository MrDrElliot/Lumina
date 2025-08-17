#pragma once
#include "Containers/Array.h"
#include "Core/Functional/Function.h"
#include "Core/Singleton/Singleton.h"
#include "entt/entt.hpp"
#include "World/Entity/Registry/EntityRegistry.h"

namespace Lumina
{
    struct SEntityComponent;
}

namespace Lumina
{
    using EntityComponentAddFn = TFunction<SEntityComponent*(entt::entity, FEntityRegistry&)>;

    
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
                    [](entt::entity e, FEntityRegistry& reg) {                    \
                        return &reg.emplace_or_replace<Type>(e);                 \
                    }                                                            \
                );                                                               \
            }                                                                    \
        };                                                                       \
        inline AutoRegister_##Type AutoRegisterInstance_##Type
}


