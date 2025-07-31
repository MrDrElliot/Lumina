#pragma once

#include "SceneTypes.h"
#include <entt/entt.hpp>
#include "Lumina.h"
#include "Core/UpdateContext.h"
#include "Core/Functional/Function.h"
#include "Core/Object/ObjectHandleTyped.h"
#include "Entity/Registry/EntityRegistry.h"
#include "Subsystems/Subsystem.h"



namespace Lumina
{
    class CEntitySystem;
    class FCameraManager;
    class FUpdateContext;
    class FSubsystemManager;
    class FTransform;
    class Entity;
    class AStaticMesh;
}

namespace Lumina
{
    
    class LUMINA_API FScene
    {
    public:
        
        FScene(ESceneType InType);
        virtual ~FScene();
        
        void Shutdown();

        void StartFrame();
        void Update(const FUpdateContext& UpdateContext);
        void EndFrame();
        
        bool RegisterSystem(CEntitySystem* NewSystem);

        Entity CreateEntity(const FTransform& Transform, const FName& Name);
        void CopyEntity(Entity& To, const Entity& From);
        void DestroyEntity(Entity Entity);
        
        FEntityRegistry& GetMutableEntityRegistry() { return EntityRegistry; }
        const FEntityRegistry& GetConstEntityRegistry() const { return EntityRegistry; }
        
        FSubsystemManager* GetSceneSubsystemManager() const { return SceneSubsystemManager; }
        FCameraManager* GetSceneCameraManager() const;

        double GetSceneDeltaTime() const { return DeltaTime; }
        double GetTimeSinceSceneCreation() const { return TimeSinceCreation; }

        void SetPaused(bool bNewPause) { bPaused = bNewPause; }
        bool IsPaused() const { return bPaused; }
        
        template<typename T>
        T* GetSceneSubsystem() const;
        
        template <typename T>
        void ForEachComponent(const TFunction<void(uint32& CurrentIndex, entt::entity& OutEntity, T& OutComponent)>&& Functor);
        ESceneType GetSceneType() const { return SceneType; }

        TVector<TObjectHandle<CEntitySystem>> GetSystemsForUpdateStage(EUpdateStage Stage);
        
    private:
        
        
        ESceneType                      SceneType;
        double                          DeltaTime = 0.0;
        double                          TimeSinceCreation = 0.0;
        bool                            bPaused = false;
        
        FSubsystemManager*                            SceneSubsystemManager = nullptr;

        TVector<TObjectHandle<CEntitySystem>>         SystemUpdateList[(int32)EUpdateStage::Max];

        FEntityRegistry                 EntityRegistry;
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
