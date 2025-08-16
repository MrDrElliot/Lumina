#pragma once
#include "Core/UpdateStage.h"
#include "Core/Math/Hash/Hash.h"
#include <entt/entt.hpp>
#include "Core/Object/Object.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "EntitySystem.generated.h"
#include "Core/UpdateContext.h"


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

        /** System ID */
        virtual uint32 GetSystemID() const { return INDEX_NONE; }

        /** Retrieves the update priority and stage for this system */
        virtual const FUpdatePriorityList* GetRequiredUpdatePriorities() { return nullptr; }

        /** Called when the system is first constructed for the scene */
        virtual void Initialize() { }

        /** Called when the system is removed from the scene, (and scene shutdown) */
        virtual void Shutdown() { }

    protected:

        /** Called per-update */
        virtual void Update(FEntityRegistry& EntityRegistry, const FUpdateContext& UpdateContext) { }

    private:

        /** We do not allow mutation of the scene through systems */
        TObjectHandle<CWorld> World;
        
    };
}


#define DEFINE_SCENE_SYSTEM(Type, ... )\
constexpr const static uint32 EntitySystemID = Lumina::Hash::FNV1a::GetHash32(#Type);\
virtual uint32 GetSystemID() const override final { return Type::EntitySystemID; }\
static const FUpdatePriorityList PriorityList; \
virtual const FUpdatePriorityList* GetRequiredUpdatePriorities() override { static const FUpdatePriorityList PriorityList = FUpdatePriorityList(__VA_ARGS__); return &PriorityList; }
