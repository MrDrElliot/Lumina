#pragma once

#include "Module/API.h"
#include "Core/Object/Class.h"
#include "Core/Functional/Function.h"
#include "entt/entt.hpp"


namespace Lumina
{
    struct SStaticMeshComponent;
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
                using namespace entt::literals; \
                Lumina::Components::RegisterComponent(#Type, [](entt::registry& reg, entt::entity e) { \
                    reg.emplace_or_replace<Type>(e); \
                }); \
                /*entt::meta_factory<Type>().func<&Type::StaticStruct>("staticstruct"_hs);*/ \
            } \
        }; \
        inline static AutoRegister_##Type AutoRegisterInstance_##Type; \
    }

