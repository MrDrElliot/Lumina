#include "Scene.h"

#include "Renderer/Material.h"
#include "SceneRenderer.h"
#include "Assets/AssetRegistry/AssetRegistry.h"
#include "Components/NameComponent.h"
#include "Components/GuidComponent.h"
#include "Entity/Entity.h"
#include "Memory/SmartPtr.h"

namespace Lumina
{
    LScene::LScene(TSharedPtr<FCamera> Camera): LAsset(FAssetMetadata())
    {
        CurrentCamera = Camera;
        SceneRenderer = FSceneRenderer::Create(this);
    }

    LScene::~LScene()
    {
        SceneRenderer->Shutdown();
    }

    TSharedPtr<LScene> LScene::Create(TSharedPtr<FCamera> Camera)
    {
        return eastl::make_shared<LScene>(Camera);
    }

    void LScene::BeginScene()
    {
        SceneRenderer->BeginScene(CurrentCamera);
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
    
    Entity LScene::CreateEntity(const FTransform& Transform, const FString& Name)
    {
        FString uniqueName = Name;
        int counter = 1;

        std::unordered_set<FString> existingNames;

        for (auto& ent : mEntityRegistery.view<FNameComponent>())
        {
            auto& existingName = mEntityRegistery.get<FNameComponent>(ent).GetName();
            existingNames.insert(existingName);
        }

        while (existingNames.count(uniqueName) > 0)
        {
            uniqueName = Name + FString("_") + eastl::to_string(counter);
            counter++;
        }

        Entity NewEntity(mEntityRegistery.create(), this);
        FGuid Guid = FGuid::Generate();
        NewEntity.AddComponent<FGUIDComponent>(Guid);
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
