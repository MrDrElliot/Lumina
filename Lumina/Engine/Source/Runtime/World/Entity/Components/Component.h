#pragma once

#include "Core/Engine/Engine.h"
#include "EntityComponentRegistry.h"
#include "entt/entt.hpp"
#include "World/Entity/Registry/EntityRegistry.h"

namespace Lumina
{
#define ENTITY_COMPONENT(Type) \
    static void* GetComponentStructType() { return Type::StaticStruct(); } \
    static void* AddComponent(entt::entity e, void* reg) { \
        return &static_cast<Lumina::FEntityRegistry*>(reg)->emplace_or_replace<Type>(e); \
    } \
    static void RegisterMeta() { \
        using namespace entt::literals; \
        entt::locator<entt::meta_ctx>::reset(GEngine->GetEngineMetaContext()); \
        entt::meta_factory<Type>{}.type(#Type ## _hs); \
        entt::meta_factory<Type>().func<&Type::GetComponentStructType>("staticstruct"_hs); \
        entt::meta_factory<Type>().func<&Type::AddComponent>("addcomponent"_hs); \
    } \
    struct DeferredAutoRegister { \
        DeferredAutoRegister() { \
            Lumina::ECS::AddDeferredComponentRegistry(&Type::RegisterMeta); \
        } \
    }; \
    static inline DeferredAutoRegister DeferredAutoRegisterInstance;
}


namespace Lumina::ECS
{
    LUMINA_API inline auto GetSharedMetaCtxHandle()
    {
        return entt::locator<entt::meta_ctx>::handle();
    }

    LUMINA_API inline void SetMetaContext(auto sharedCtx)
    {
        entt::locator<entt::meta_ctx>::reset(sharedCtx);
    }
}
