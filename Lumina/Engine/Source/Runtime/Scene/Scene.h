#pragma once

#include <entt/entt.hpp>
#include "Camera.h"
#include "Assets/Asset.h"


class FTransform;
class LString;

namespace Lumina
{
    struct FSceneSettings
    {
        FSceneSettings()
        {
            BackgroundColor = {0.0f, 0.0f, 0.0f};
            bShowGrid = true;
        }
        glm::vec3 BackgroundColor;
        bool bShowGrid;
    };
    
    class Material;
    class LStaticMesh;
    class Entity;
    class FSceneRenderer;
    
    class LScene : public LAsset
    {
    public:

        using EntityRegistry = entt::registry;
        
        LScene();
        ~LScene();

        void BeginScene();
        void OnUpdate(double DeltaTime);
        void EndScene();
        void Shutdown();

        std::shared_ptr<FCamera> GetEditorCamera() { return EditorCamera; }
        std::shared_ptr<FSceneRenderer> GetSceneRenderer() { return SceneRenderer; }
        

        Entity CreateEntity(const FTransform& Transform, const LString& Name);
        void DestroyEntity(Entity Entity);
        Entity GetEntityByGUID(const FGuid& Guid, bool* bFound = nullptr);
        
        EntityRegistry& GetEntityRegistry() { return mEntityRegistery; }
        FSceneSettings& GetSceneSettings() { return Settings; }


        template <typename T>
        void ForEachComponent(const std::function<void(uint32& NumEntires, uint32& CurrentIndex, entt::entity& OutEntity, T& OutComponent)>& Functor)
        {
            auto view = mEntityRegistery.view<T>();

            uint32 NumEntires = view.end().index();
            uint32 Current = 0;
            for (auto [entity, component] : view.each())
            {
                Functor(NumEntires, Current, entity, component);
                Current++;
            }
        }
    
    private:
        
        FSceneSettings Settings;
        EntityRegistry mEntityRegistery;
        std::unordered_map<FGuid, Entity> EntityIdentifierMap;
        
        std::shared_ptr<FCamera>        EditorCamera;
        std::shared_ptr<FSceneRenderer> SceneRenderer;

    };
}
