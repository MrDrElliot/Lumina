

#include "Scene.h"
#include "Renderer/RHIIncl.h"
#include "SceneRenderer.h"
#include "SceneUpdateContext.h"
#include "Containers/Name.h"
#include "Core/Profiler/Profile.h"
#include "Entity/Entity.h"
#include "Entity/Components/NameComponent.h"
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
        
    }

    void FScene::StartFrame()
    {
    }
    
    void FScene::Update(const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();
        
        DeltaTime = UpdateContext.GetDeltaTime();
        TimeSinceCreation += DeltaTime;
        
        FSceneUpdateContext SceneContext(UpdateContext, this);
        for (FEntitySystem* System : SystemUpdateList[(uint32)UpdateContext.GetUpdateStage()])
        {
            System->Update(EntityRegistry, SceneContext);
        }
        
    }
    
    void FScene::EndFrame()
    {
    }

    bool FScene::RegisterSystem(FEntitySystem* NewSystem)
    {
        Assert(NewSystem != nullptr)
        
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

    Entity FScene::CreateEntity(const FTransform& Transform, const FName& Name)
    {
        auto GenerateUniqueName = [] (const FInlineString& baseName, int32 counterValue)
        {
            FInlineString finalName;

            if (baseName.length() > 3)
            {
                // Check if the last three characters are a numeric set, if so then increment the value and replace them
                if (isdigit( baseName[baseName.length() - 1]) && isdigit(baseName[baseName.length() - 2]) && isdigit(baseName[baseName.length() - 3]))
                {
                    finalName.sprintf("%s%03u", baseName.substr(0, baseName.length() - 3).c_str(), counterValue);
                    return finalName;
                }
            }

            finalName.sprintf( "%s %03u", baseName.c_str(), counterValue );
            return finalName;
        };

        //-------------------------------------------------------------------------

        FInlineString desiredName = Name.c_str();
        FInlineString finalName = desiredName;
        FName FinalNameID(finalName.c_str());

        uint32 counter = 1;
        bool isUniqueName = false;

        while (!isUniqueName)
        {
            isUniqueName = EntityNameMap.find(FinalNameID) == EntityNameMap.end();

            if (!isUniqueName)
            {
                finalName = GenerateUniqueName(desiredName, counter++);
                FinalNameID = FName(finalName.c_str());
            }
        }

        entt::entity NewEntity = EntityRegistry.create();
        EntityRegistry.emplace<SNameComponent>(NewEntity).Name = FinalNameID;
        EntityRegistry.emplace<STransformComponent>(NewEntity).Transform = Transform;
        
        EntityNameMap.insert(TPair<FName, entt::entity>(FinalNameID, NewEntity));

        return Entity(NewEntity, this);
    }


    
    void FScene::DestroyEntity(Entity Entity)
    {
        EntityNameMap.erase(EntityNameMap.find(Entity.GetName()));
        EntityRegistry.destroy(Entity);
    }

    FCameraManager* FScene::GetSceneCameraManager() const
    {
        return SceneSubsystemManager->GetSubsystem<FCameraManager>();
    }
    
}
