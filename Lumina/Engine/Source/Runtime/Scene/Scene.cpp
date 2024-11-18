#include "Scene.h"

#include "Renderer/Material.h"
#include "SceneRenderer.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Components/NameComponent.h"
#include "Components/UniqueComponent.h"
#include "Entity/Entity.h"

namespace Lumina
{
    LScene::LScene(): LAsset(FAssetMetadata())
    {
        EditorCamera = std::make_shared<FCamera>();
        SceneRenderer = FSceneRenderer::Create(this);
    }

    LScene::~LScene()
    {
        
    }

    void LScene::BeginScene()
    {
        SceneRenderer->BeginScene(EditorCamera);
    }

    void LScene::OnUpdate(double DeltaTime)
    {
        BeginScene();
        EndScene();
    }

    void LScene::EndScene()
    {
        SceneRenderer->EndScene();
    }
    
    void LScene::Shutdown()
    {
        SceneRenderer->Shutdown();
    }

    Entity LScene::CreateEntity(const FTransform& Transform, const LString& Name)
    {
        // Check for existing entities with the same name and modify the name if necessary
        LString uniqueName = Name;
        int counter = 1;

        // Create a set to track the names we've encountered so far
        std::unordered_set<LString> existingNames;

        // Add existing names to the set
        for (auto& ent : mEntityRegistery.view<FNameComponent>())
        {
            auto& existingName = mEntityRegistery.get<FNameComponent>(ent).GetName();
            existingNames.insert(existingName);
        }

        // If the name already exists, append a number to make it unique
        while (existingNames.count(uniqueName) > 0)
        {
            uniqueName = Name + LString("_") + std::to_string(counter);
            counter++;
        }

        // Now, create the new entity with the unique name
        Entity NewEntity(mEntityRegistery.create(), this);
        FGuid Guid = FGuid::Generate();
        NewEntity.AddComponent<FUniqueComponent>(Guid);
        NewEntity.AddComponent<FNameComponent>(uniqueName);
        NewEntity.AddComponent<FTransformComponent>(Transform);

        EntityIdentifierMap[Guid] = NewEntity;

        return NewEntity;
    }
    
    void LScene::DestroyEntity(Entity Entity)
    {
        mEntityRegistery.destroy(Entity);
    }

    Entity LScene::GetEntityByGUID(const FGuid& Guid, bool* bFound)
    {
        if (EntityIdentifierMap.find(Guid) != EntityIdentifierMap.end())
        {
            if (bFound)
            {
                *bFound = true;
            }
            return EntityIdentifierMap.at(Guid);
        }
    
        if (bFound)
        {
            *bFound = false;
        }
        return Entity();
    }

}
