#pragma once

#include <entt/entt.hpp>
#include "Camera.h"
#include "Assets/Asset.h"
#include "EASTL/shared_ptr.h"
#include "EASTL/unordered_map.h"
#include "GUID/GUID.h"
#include "Memory/SmartPtr.h"



namespace Lumina
{
    class FTransform;
    class Entity;
    class Material;
    class AStaticMesh;
    class FSceneRenderer;

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
    
    
    class AScene : public IAsset
    {
    public:

        using EntityRegistry = entt::registry;

        AScene() = default;
        AScene(TSharedPtr<FCamera> Camera);
        ~AScene();

        DECLARE_ASSET("Scene", Scene, 1);

        void Serialize(FArchive& Ar) override {}
        
        static TSharedPtr<AScene> Create(TSharedPtr<FCamera> Camera);

        void BeginScene();
        void OnUpdate(double DeltaTime);
        void EndScene();

        TSharedPtr<FCamera> GetCurrentCamera() { return CurrentCamera; }
        TSharedPtr<FSceneRenderer> GetSceneRenderer() { return SceneRenderer; }
        

        Entity CreateEntity(const FTransform& Transform, const FString& Name);
        void DestroyEntity(Entity Entity);
        Entity GetEntityByGUID(const FGuid& Guid, bool* bFound = nullptr);
        
        EntityRegistry& GetEntityRegistry() { return mEntityRegistery; }
        FSceneSettings& GetSceneSettings() { return Settings; }


        template <typename T>
        void ForEachComponent(const eastl::function<void(uint32& CurrentIndex, entt::entity& OutEntity, T& OutComponent)>&& Functor)
        {
            auto view = mEntityRegistery.view<T>();
            
            uint32 Current = 0;
            for (auto [entity, component] : view.each())
            {
                Functor(Current, entity, component);
                Current++;
            }
        }
    
    private:
        
        FSceneSettings Settings;
        EntityRegistry mEntityRegistery;
        
        TSharedPtr<FCamera>        CurrentCamera;
        TSharedPtr<FSceneRenderer> SceneRenderer;

    };
}
