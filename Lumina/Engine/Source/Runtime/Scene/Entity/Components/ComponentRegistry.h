#pragma once
#include "Containers/Name.h"
#include "Core/Functional/Function.h"
#include "entt/entt.hpp"
#include "Module/API.h"


namespace Lumina
{
    class CStruct;
}

namespace Lumina::Components
{
    using ComponentAddFn = TFunction<void(entt::registry&, entt::entity)>;

    LUMINA_API void RegisterComponent(const char* Struct, ComponentAddFn Fn);
    LUMINA_API ComponentAddFn GetEntityComponentCreationFn(CStruct* Struct);
    
}

#define DECLARE_ENTITY_COMPONENT(Type) \
    namespace \
    { \
        struct AutoRegister_##Type { \
            AutoRegister_##Type() { \
                Lumina::Components::RegisterComponent(#Type, [](entt::registry& reg, entt::entity e) { \
                    reg.emplace_or_replace<Type>(e); \
                }); \
            } \
        }; \
        inline static AutoRegister_##Type AutoRegisterInstance_##Type; \
    }
