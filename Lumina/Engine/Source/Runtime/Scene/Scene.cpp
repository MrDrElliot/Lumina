

#include "Scene.h"
#include "SceneRenderer.h"
#include "SceneUpdateContext.h"
#include "Entity/Entity.h"
#include "Entity/Components/NameComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Systems/EntitySystem.h"
#include "Subsystems/FCameraManager.h"

namespace Lumina
{
    FScene::FScene(ESceneType InType)
        : SceneType(InType)
    {
    }

    FScene::~FScene()
    {
    }
    
    void FScene::Initialize(const FUpdateContext& UpdateContext)
    {
        SystemManager = UpdateContext.GetSubsystemManager();
        
        SceneSubsystemManager = FMemory::New<FSubsystemManager>();
        SceneRenderer = SceneSubsystemManager->AddSubsystem<FSceneRenderer>(this);
        SceneSubsystemManager->AddSubsystem<FCameraManager>();
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
        FSceneUpdateContext SceneContext(UpdateContext, this);
        
        for (FEntitySystem* System : SystemUpdateList[(uint32)UpdateContext.GetUpdateStage()])
        {
            System->Update(SceneEntityRegistery, SceneContext);
        }
    }
    
    void FScene::EndFrame()
    {
        SceneRenderer->EndFrame();
    }

    bool FScene::RegisterSystem(FEntitySystem* NewSystem)
    {
        Assert(NewSystem != nullptr);
        
        NewSystem->Scene = this;
        NewSystem->Initialize(SceneSubsystemManager);
        EntitySystems.push_back(NewSystem);

        for (uint8 i = 0; i < (uint8)EUpdateStage::Max; ++i)
        {
            if (NewSystem->GetRequiredUpdatePriorities().IsStageEnabled((EUpdateStage)i))
            {
                SystemUpdateList[i].push_back(NewSystem);
            }

            auto Predicate = [i] (FEntitySystem* A, FEntitySystem* B)
            {
                const uint8 PriorityA = A->GetRequiredUpdatePriorities().GetPriorityForStage((EUpdateStage)i);
                const uint8 PriorityB = B->GetRequiredUpdatePriorities().GetPriorityForStage((EUpdateStage)i);
                return PriorityA > PriorityB;
            };

            eastl::sort(SystemUpdateList[i].begin(), SystemUpdateList[i].end(), Predicate);
        }

        return true;
    }

    Entity FScene::CreateEntity(const FTransform& Transform, const FString& Name)
    {
        FString uniqueName = Name;
        int Counter = 1;

        bool bNameExists = false;
        for (auto& ent : SceneEntityRegistery.view<FNameComponent>())
        {
            auto& existingName = SceneEntityRegistery.get<FNameComponent>(ent).GetName();
            if (existingName == uniqueName)
            {
                bNameExists = true;
                break;
            }
        }

        while (bNameExists)
        {
            uniqueName = Name + FString("_") + eastl::to_string(Counter);
            Counter++;

            bNameExists = false;
            for (auto& ent : SceneEntityRegistery.view<FNameComponent>())
            {
                auto& existingName = SceneEntityRegistery.get<FNameComponent>(ent).GetName();
                if (existingName == uniqueName)
                {
                    bNameExists = true;
                    break;
                }
            }
        }

        Entity NewEntity(SceneEntityRegistery.create(), this);
        NewEntity.AddComponent<FNameComponent>(uniqueName);
        NewEntity.AddComponent<FTransformComponent>(Transform);

        
        return NewEntity;
    }

    
    void FScene::DestroyEntity(Entity Entity)
    {
        SceneEntityRegistery.destroy(Entity);
    }

    FCameraManager* FScene::GetSceneCameraManager() const
    {
        return SceneSubsystemManager->GetSubsystem<FCameraManager>();
    }
}
