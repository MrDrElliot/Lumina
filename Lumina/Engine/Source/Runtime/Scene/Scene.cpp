

#include "Scene.h"
#include "SceneRenderer.h"
#include "Components/NameComponent.h"
#include "Components/GuidComponent.h"
#include "Entity/Entity.h"

namespace Lumina
{
    FScene::FScene(ESceneType InType)
    {
        SceneType = InType;
        CurrentCamera = MakeRefPtr<FCamera>();
        SceneRenderer = FSceneRenderer::Create(this);
    }

    FScene::~FScene()
    {
    }

    TRefPtr<FScene> FScene::Create(ESceneType InType)
    {
        return MakeRefPtr<FScene>(InType);
    }
    
    void FScene::Initialize(const FUpdateContext& UpdateContext)
    {
        SystemManager = UpdateContext.GetSubsystemManager();
    }

    void FScene::Shutdown()
    {
        SceneRenderer->Shutdown();
        
        SystemManager = nullptr;
    }

    void FScene::StartFrame()
    {
        SceneRenderer->StartFrame();
    }
    
    void FScene::Update(const FUpdateContext& UpdateContext)
    {
    }
    
    void FScene::EndFrame()
    {
        SceneRenderer->EndFrame();
    }

    TRefPtr<FSceneRenderer> FScene::GetSceneRenderer() const
    {
        return SceneRenderer;
    }

    Entity FScene::CreateEntity(const FTransform& Transform, const FString& Name)
    {
        FString uniqueName = Name;
        int counter = 1;

        bool nameExists = false;
        for (auto& ent : EntityRegistery.view<FNameComponent>())
        {
            auto& existingName = EntityRegistery.get<FNameComponent>(ent).GetName();
            if (existingName == uniqueName)
            {
                nameExists = true;
                break;
            }
        }

        while (nameExists)
        {
            uniqueName = Name + FString("_") + eastl::to_string(counter);
            counter++;

            nameExists = false;
            for (auto& ent : EntityRegistery.view<FNameComponent>())
            {
                auto& existingName = EntityRegistery.get<FNameComponent>(ent).GetName();
                if (existingName == uniqueName)
                {
                    nameExists = true;
                    break;
                }
            }
        }

        Entity NewEntity(EntityRegistery.create(), this);
        FGuid Guid = FGuid::Generate();
        NewEntity.AddComponent<FGUIDComponent>(Guid);
        NewEntity.AddComponent<FNameComponent>(uniqueName);
        NewEntity.AddComponent<FTransformComponent>(Transform);
        
        return NewEntity;
    }

    
    void FScene::DestroyEntity(Entity Entity)
    {
        EntityRegistery.destroy(Entity);
    }

    Entity FScene::GetEntityByGUID(const FGuid& Guid, bool* bFound)
    {
        auto View = EntityRegistery.view<FGUIDComponent>();

        for (auto& ent : View)
        {
            if (View.get<FGUIDComponent>(ent).GetGUID() == Guid)
            {
                if (bFound)
                {
                    *bFound = true;
                }
                return Entity(ent, this);
            }
        }
        
        return {};
    }
}
