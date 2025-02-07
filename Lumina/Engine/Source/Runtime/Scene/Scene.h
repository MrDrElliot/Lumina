#pragma once

#include <entt/entt.hpp>
#include "Camera.h"
#include "SceneTypes.h"
#include "Assets/Asset.h"
#include "EASTL/unordered_map.h"
#include "GUID/GUID.h"


namespace Lumina
{
    class FUpdateContext;
}

namespace Lumina
{
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
        
        static TRefPtr<FScene> Create(ESceneType InType);


        void Initialize(const FUpdateContext& UpdateContext);
        void Shutdown();

        void StartFrame();
        void Update(const FUpdateContext& UpdateContext);
        void EndFrame();
        
        FORCEINLINE TRefPtr<FCamera> GetCurrentCamera() { return CurrentCamera; }
        FORCEINLINE TRefPtr<FSceneRenderer> GetSceneRenderer() const;
        

        Entity CreateEntity(const FTransform& Transform, const FString& Name);
        void DestroyEntity(Entity Entity);
        Entity GetEntityByGUID(const FGuid& Guid, bool* bFound = nullptr);
        
        EntityRegistry& GetEntityRegistry() { return EntityRegistery; }
        FSceneSettings& GetSceneSettings() { return Settings; }


        template <typename T>
        void ForEachComponent(const eastl::function<void(uint32& CurrentIndex, entt::entity& OutEntity, T& OutComponent)>&& Functor)
        {
            auto view = EntityRegistery.view<T>();
            
            uint32 Current = 0;
            for (auto [entity, component] : view.each())
            {
                Functor(Current, entity, component);
                Current++;
            }
        }
    
    private:

        const FSubsystemManager*        SystemManager = nullptr;
        ESceneType                      SceneType;
        
        FSceneSettings                  Settings;
        EntityRegistry                  EntityRegistery;
        
        TRefPtr<FCamera>                CurrentCamera;
        TRefPtr<FSceneRenderer>         SceneRenderer;

    };

}
