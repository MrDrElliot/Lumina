#pragma once
#include "Core/UpdateStage.h"
#include "Core/Math/Hash/Hash.h"
#include <entt/entt.hpp>
#include "Core/Object/Object.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "Core/UpdateContext.h"
#include "EntitySystem.generated.h"


namespace Lumina
{
    class FEntityRegistry;
    class Entity;
    class FSubsystemManager;
}

namespace Lumina
{

    LUM_CLASS()
    class LUMINA_API CEntitySystemRegistry : public CObject
    {
        GENERATED_BODY()
    public:

        void RegisterSystem(class CEntitySystem* NewSystem);

        static CEntitySystemRegistry& Get();

        void GetRegisteredSystems(TVector<TObjectHandle<CEntitySystem>>& Systems);

        TVector<TObjectHandle<class CEntitySystem>> RegisteredSystems;

        static CEntitySystemRegistry* Singleton;

    };
    

    LUM_CLASS()
    class LUMINA_API CEntitySystem : public CObject
    {
        GENERATED_BODY()
        
    public:

        friend class CWorld;
        
        void PostCreateCDO() override;
        
        /** Retrieves the update priority and stage for this system */
        virtual const FUpdatePriorityList* GetRequiredUpdatePriorities() { return nullptr; }

        /** Called when the system is first constructed for the world */
        virtual void Initialize() { }

        /** Called per-update, for each required system */
        virtual void Update(FEntityRegistry& EntityRegistry, const FUpdateContext& UpdateContext) { }

        /** Called when the system is removed from the world, (and world shutdown) */
        virtual void Shutdown() { }
    
    private:

        /** We do not allow mutation of the scene through systems */
        TObjectHandle<CWorld> World;
        
    };
}


#define DEFINE_SCENE_SYSTEM(Type, ... )\
static const FUpdatePriorityList PriorityList; \
virtual const FUpdatePriorityList* GetRequiredUpdatePriorities() override { static const FUpdatePriorityList PriorityList = FUpdatePriorityList(__VA_ARGS__); return &PriorityList; }
