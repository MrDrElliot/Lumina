#pragma once

#include "SceneTypes.h"
#include <entt/entt.hpp>
#include "Lumina.h"
#include "Core/UpdateContext.h"
#include "Core/Functional/Function.h"
#include "Entity/Registry/EntityRegistry.h"
#include "Subsystems/Subsystem.h"


namespace Lumina
{
    class FCameraManager;
    class FUpdateContext;
    class FSubsystemManager;
    class FTransform;
    class Entity;
    class Material;
    class AStaticMesh;
    class FEntitySystem;
}

namespace Lumina
{
    
    class FScene
    {
    public:
        
        FScene(ESceneType InType);
        virtual ~FScene();
        
        void Shutdown();

        void StartFrame();
        void Update(const FUpdateContext& UpdateContext);
        void EndFrame();
        
        bool RegisterSystem(FEntitySystem* NewSystem);

        Entity CreateEntity(const FTransform& Transform, const FName& Name);
        void DestroyEntity(Entity Entity);
        
        FEntityRegistry& GetMutableEntityRegistry() { return EntityRegistry; }
        const FEntityRegistry& GetConstEntityRegistry() const { return EntityRegistry; }
        
        FORCEINLINE FSubsystemManager* GetSceneSubsystemManager() const { return SceneSubsystemManager; }
        FCameraManager* GetSceneCameraManager() const;

        FORCEINLINE double GetSceneDeltaTime() const { return DeltaTime; }

        template<typename T>
        T* GetSceneSubsystem() const;
        

        template <typename T>
        void ForEachComponent(const TFunction<void(uint32& CurrentIndex, entt::entity& OutEntity, T& OutComponent)>&& Functor);

        FORCEINLINE ESceneType GetSceneType() const { return SceneType; }
    
    private:
        
        
        ESceneType                      SceneType;
        double                          DeltaTime = 0.0;
        
        FSubsystemManager*              SceneSubsystemManager = nullptr;

        TVector<FEntitySystem*>         EntitySystems;
        TVector<FEntitySystem*>         SystemUpdateList[(uint32)EUpdateStage::Max];
        FEntityRegistry                 EntityRegistry;

        THashMap<FName, entt::entity>   EntityNameMap;
    };


    //--------------------------------------------------------------------------------------------------
    // Templates
    //--------------------------------------------------------------------------------------------------

    template <typename T>
    T* FScene::GetSceneSubsystem() const
    {
        return GetSceneSubsystemManager()->GetSubsystem<T>();
    }

    template <typename T>
    void FScene::ForEachComponent(const eastl::function<void(uint32& CurrentIndex, entt::entity& OutEntity, T& OutComponent)>&& Functor)
    {
        auto view = EntityRegistry.view<T>();
        
        uint32 Current = 0;
        for (auto [entity, component] : view.each())
        {
            Functor(Current, entity, component);
            Current++;
        }
    }
}
