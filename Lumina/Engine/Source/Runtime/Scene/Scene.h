#pragma once

#include "SceneTypes.h"
#include "Assets/Asset.h"
#include "glm/glm.hpp"
#include <entt/entt.hpp>

#include "Core/UpdateContext.h"
#include "Core/Functional/Function.h"
#include "Subsystems/Subsystem.h"


namespace Lumina
{
    class FEntitySystem;
}

namespace Lumina
{
    class FCameraManager;
    class FUpdateContext;
    class FSceneRenderer;
    class FSubsystemManager;
    class FTransform;
    class Entity;
    class Material;
    class AStaticMesh;

    struct FSceneSettings
    {
        FSceneSettings()
        {
            BackgroundColor = {0.24f, 0.24f, 0.24f};
            bShowGrid = true;
        }

        glm::vec3 BackgroundColor;
        bool bShowGrid;
    };
    
    
    class FScene : public FRefCounted
    {
    public:

        using EntityRegistry = entt::registry;

        FScene(ESceneType InType);
        ~FScene();
        

        void Initialize(const FUpdateContext& UpdateContext);
        void Shutdown();

        void StartFrame();
        void Update(const FUpdateContext& UpdateContext);
        void EndFrame();
        
        bool RegisterSystem(FEntitySystem* NewSystem);

        Entity CreateEntity(const FTransform& Transform, const FString& Name);
        void DestroyEntity(Entity Entity);
        
        EntityRegistry& GetEntityRegistry() { return SceneEntityRegistery; }
        FSceneSettings& GetSceneSettings() { return Settings; }

        FORCEINLINE FSubsystemManager* GetSceneSubsystemManager() const { return SceneSubsystemManager; }
        FCameraManager* GetSceneCameraManager() const;

        FORCEINLINE FSceneRenderer* GetSceneRenderer() const { return SceneRenderer; }


        template<typename T>
        T* GetSceneSubsystem() const;
        

        template <typename T>
        void ForEachComponent(const TFunction<void(uint32& CurrentIndex, entt::entity& OutEntity, T& OutComponent)>&& Functor);

    

        
    private:
        
        FSceneSettings                  Settings;
        ESceneType                      SceneType;

        const FSubsystemManager*        SystemManager = nullptr;
        
        FSubsystemManager*              SceneSubsystemManager = nullptr;

        TVector<FEntitySystem*>         EntitySystems;
        TVector<FEntitySystem*>         SystemUpdateList[(uint32)EUpdateStage::Max];
        EntityRegistry                  SceneEntityRegistery;
        
        FSceneRenderer*                 SceneRenderer = nullptr;

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
        auto view = SceneEntityRegistery.view<T>();
        
        uint32 Current = 0;
        for (auto [entity, component] : view.each())
        {
            Functor(Current, entity, component);
            Current++;
        }
    }
}
