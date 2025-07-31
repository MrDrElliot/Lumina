
#include "Scene.h"
#include "Renderer/RHIIncl.h"
#include "SceneRenderer.h"
#include "SceneUpdateContext.h"
#include "Containers/Name.h"
#include "Core/Profiler/Profile.h"
#include "Entity/Entity.h"
#include "Entity/Components/NameComponent.h"
#include "Entity/Components/StaticMeshComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Systems/EntitySystem.h"
#include "Subsystems/FCameraManager.h"
#include "TaskSystem/TaskSystem.h"

namespace Lumina
{
    FScene::FScene(ESceneType InType)
        : SceneType(InType)
    {
        SceneSubsystemManager = Memory::New<FSubsystemManager>();
        SceneSubsystemManager->AddSubsystem<FCameraManager>();
    }

    FScene::~FScene()
    {
    }

    void FScene::Shutdown()
    {
        for (uint8 i = 0; i < (uint8)EUpdateStage::Max; i++)
        {
            for (CEntitySystem* System : SystemUpdateList[i])
            {
                System->Shutdown();
                System->MarkGarbage();
            }
        }
        SystemUpdateList->clear();
    }

    void FScene::StartFrame()
    {
    }
    
    void FScene::Update(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();

        if (UpdateContext.GetUpdateStage() == EUpdateStage::FrameStart)
        {
            DeltaTime = UpdateContext.GetDeltaTime();
            TimeSinceCreation += DeltaTime;
        }
        
        FSceneUpdateContext SceneContext(UpdateContext, this);
        
        for (CEntitySystem* System : SystemUpdateList[(uint32)UpdateContext.GetUpdateStage()])
        {
            System->Update(EntityRegistry, SceneContext);
        }
        
    }
    
    void FScene::EndFrame()
    {
    }

    bool FScene::RegisterSystem(CEntitySystem* NewSystem)
    {
        Assert(NewSystem != nullptr)
        
        NewSystem->Scene = this;
        NewSystem->Initialize(SceneSubsystemManager);

        for (uint8 i = 0; i < (uint8)EUpdateStage::Max; ++i)
        {
            if (NewSystem->GetRequiredUpdatePriorities()->IsStageEnabled((EUpdateStage)i))
            {
                SystemUpdateList[i].push_back(NewSystem);
            }

            auto Predicate = [i] (CEntitySystem* A, CEntitySystem* B)
            {
                const uint8 PriorityA = A->GetRequiredUpdatePriorities()->GetPriorityForStage((EUpdateStage)i);
                const uint8 PriorityB = B->GetRequiredUpdatePriorities()->GetPriorityForStage((EUpdateStage)i);
                return PriorityA > PriorityB;
            };

            eastl::sort(SystemUpdateList[i].begin(), SystemUpdateList[i].end(), Predicate);
        }

        return true;
    }

    Entity FScene::CreateEntity(const FTransform& Transform, const FName& Name)
    {
        entt::entity NewEntity = EntityRegistry.create();
        EntityRegistry.emplace<SNameComponent>(NewEntity).Name = Name;
        EntityRegistry.emplace<STransformComponent>(NewEntity).Transform = Transform;
        
        return Entity(NewEntity, this);
    }

    void FScene::CopyEntity(Entity& To, const Entity& From)
    {
        entt::entity NewEntity = EntityRegistry.create();
        To = Entity(NewEntity, this);
        
        for (auto [id, storage]: EntityRegistry.storage())
        {
            if(storage.contains(From.GetHandle()))
            {
                storage.push(To.GetHandle(), storage.value(From.GetHandle()));
            }
        }
    }

    void FScene::DestroyEntity(Entity Entity)
    {
        EntityRegistry.destroy(Entity);
    }

    FCameraManager* FScene::GetSceneCameraManager() const
    {
        return SceneSubsystemManager->GetSubsystem<FCameraManager>();
    }

    TVector<TObjectHandle<CEntitySystem>> FScene::GetSystemsForUpdateStage(EUpdateStage Stage)
    {
        return SystemUpdateList[uint32(Stage)];
    }
}
