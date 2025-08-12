
#include "Scene.h"
#include "Renderer/RHIIncl.h"
#include "SceneRenderer.h"
#include "SceneUpdateContext.h"
#include "Containers/Name.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/matrix_decompose.hpp"
#include "Core/Profiler/Profile.h"
#include "Entity/Entity.h"
#include "Entity/Components/NameComponent.h"
#include "Entity/Components/RelationshipComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Entity/Systems/EntitySystem.h"
#include "Entity/Systems/UpdateTransformEntitySystem.h"
#include "Subsystems/FCameraManager.h"
#include "TaskSystem/TaskSystem.h"

namespace Lumina
{
    FScene::FScene(ESceneType InType)
        : SceneType(InType)
    {
        SceneSubsystemManager = Memory::New<FSubsystemManager>();
        SceneSubsystemManager->AddSubsystem<FCameraManager>();
        RegisterSystem(NewObject<CUpdateTransformEntitySystem>());
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
 
    void FScene::ReparentEntity(Entity Child, Entity Parent)
    {
        if (Child.GetHandle() == Parent.GetHandle())
        {
            LOG_ERROR("Cannot parent an entity to itself!");
            return;
        }
    
        // Step 1: Get child's current world transform matrix
        glm::mat4 childWorldMatrix = Child.GetWorldTransform().GetMatrix();
    
        // Step 2: Get parent's world transform matrix
        glm::mat4 parentWorldMatrix = glm::mat4(1.0f); // Identity if no parent
        if (Parent.IsValid())
        {
            parentWorldMatrix = Parent.GetWorldTransform().GetMatrix();
        }
    
        // Step 3: Calculate child's new local matrix relative to new parent
        glm::mat4 newLocalMatrix = glm::inverse(parentWorldMatrix) * childWorldMatrix;
    
        // Step 4: Decompose newLocalMatrix
        glm::vec3 translation, scale, skew;
        glm::quat rotation;
        glm::vec4 perspective;
    
        glm::decompose(newLocalMatrix, scale, rotation, translation, skew, perspective);
    
        // Step 5: Update child's local transform component
        if (Child.HasComponent<STransformComponent>())
        {
            Child.Patch<STransformComponent>([&](auto& Transform)
            {
                Transform.SetLocation(translation);
                Transform.SetRotation(rotation);
                Transform.SetScale(scale);
            });
        }
        else
        {
            // If no transform component, optionally add one here
            // Child.AddComponent<STransformComponent>(translation, rotation, scale);
        }
    
        // Now proceed with your existing parenting logic
        SRelationshipComponent& ParentRelationshipComponent = Parent.GetOrAddComponent<SRelationshipComponent>();
        if (ParentRelationshipComponent.Size >= SRelationshipComponent::MaxChildren)
        {
            LOG_ERROR("Parent has reached its max children");
            return;
        }
    
        SRelationshipComponent& ChildRelationshipComponent = Child.GetOrAddComponent<SRelationshipComponent>();
        if (ChildRelationshipComponent.Parent.IsValid())
        {
            if (SRelationshipComponent* ToRemove = ChildRelationshipComponent.Parent.TryGetComponent<SRelationshipComponent>())
            {
                for (SIZE_T i = 0; i < ToRemove->Size; ++i)
                {
                    if (ToRemove->Children[i] == Child)
                    {
                        // Shift remaining children down to fill the gap
                        for (SIZE_T j = i; j < ToRemove->Size - 1; ++j)
                        {
                            ToRemove->Children[j] = ToRemove->Children[j + 1];
                        }
    
                        --ToRemove->Size;
                        break; // Child found and removed, exit loop
                    }
                }
            }
        }
    
        ParentRelationshipComponent.Children[ParentRelationshipComponent.Size++] = Child;
        ChildRelationshipComponent.Parent = Parent;
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
