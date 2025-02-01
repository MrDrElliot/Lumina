

#include "Scene.h"
#include "SceneRenderer.h"
#include "Components/NameComponent.h"
#include "Components/GuidComponent.h"
#include "Entity/Entity.h"
#include "Memory/SmartPtr.h"

namespace Lumina
{
    AScene::AScene(TSharedPtr<FCamera> Camera)
    {
        CurrentCamera = Camera;
        SceneRenderer = FSceneRenderer::Create(this);
    }

    AScene::~AScene()
    {
        SceneRenderer->Shutdown();
    }

    TSharedPtr<AScene> AScene::Create(TSharedPtr<FCamera> Camera)
    {
        return MakeSharedPtr<AScene>(Camera);
    }

    void AScene::BeginScene()
    {
        SceneRenderer->BeginScene(CurrentCamera);
    }

    void AScene::OnUpdate(double DeltaTime)
    {
        BeginScene();
        EndScene();
    }

    void AScene::EndScene()
    {
        SceneRenderer->EndScene();
    }
    
    Entity AScene::CreateEntity(const FTransform& Transform, const FString& Name)
    {
        FString uniqueName = Name;
        int counter = 1;

        bool nameExists = false;
        for (auto& ent : mEntityRegistery.view<FNameComponent>())
        {
            auto& existingName = mEntityRegistery.get<FNameComponent>(ent).GetName();
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
            for (auto& ent : mEntityRegistery.view<FNameComponent>())
            {
                auto& existingName = mEntityRegistery.get<FNameComponent>(ent).GetName();
                if (existingName == uniqueName)
                {
                    nameExists = true;
                    break;
                }
            }
        }

        Entity NewEntity(mEntityRegistery.create(), this);
        FGuid Guid = FGuid::Generate();
        NewEntity.AddComponent<FGUIDComponent>(Guid);
        NewEntity.AddComponent<FNameComponent>(uniqueName);
        NewEntity.AddComponent<FTransformComponent>(Transform);
        
        return NewEntity;
    }

    
    void AScene::DestroyEntity(Entity Entity)
    {
        mEntityRegistery.destroy(Entity);
    }

    Entity AScene::GetEntityByGUID(const FGuid& Guid, bool* bFound)
    {
        auto View = mEntityRegistery.view<FGUIDComponent>();

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
