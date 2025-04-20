#pragma once
#include "Core/UpdateStage.h"
#include "Core/Math/Hash/Hash.h"
#include <entt/entt.hpp>

namespace Lumina
{
    class FEntityRegistry;
    class Entity;
    class FSceneUpdateContext;
    class FSubsystemManager;
}

namespace Lumina
{
    
    class LUMINA_API FEntitySystem
    {
    public:

        friend class FScene;
        
        FEntitySystem() = default;
        virtual ~FEntitySystem() = default;

        virtual uint32 GetSystemID() const = 0;

        /** Retrieves the update priority and stage for this system */
        virtual const FUpdatePriorityList& GetRequiredUpdatePriorities() = 0;

        /** Called when the system is first constructed for the scene */
        virtual void Initialize(const FSubsystemManager* SubsystemManager) { }

        /** Called when the system is removed from the scene, (and scene shutdown) */
        virtual void Shutdown() { }

    protected:

        /** Called per-update */
        virtual void Update(FEntityRegistry& EntityRegistry, const FSceneUpdateContext& UpdateContext) = 0;

    private:

        /** We do not allow mutation of the scene through systems */
        FScene* Scene = nullptr;
        
    };
}


#define DEFINE_SCENE_SYSTEM(Type, ... )\
constexpr const static uint32 s_entitySystemID = Lumina::Hash::FNV1a::GetHash32( #Type );\
virtual uint32 GetSystemID() const override final { return Type::s_entitySystemID; }\
static const FUpdatePriorityList s_priorityList; \
virtual const FUpdatePriorityList& GetRequiredUpdatePriorities() override { static const FUpdatePriorityList s_priorityList = FUpdatePriorityList(__VA_ARGS__); return s_priorityList; }
