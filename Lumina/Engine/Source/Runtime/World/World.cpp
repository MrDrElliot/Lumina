#include "World.h"

#include "WorldManager.h"
#include "Core/Engine/Engine.h"
#include "Core/Object/Class.h"
#include "Entity/Components/EditorComponent.h"
#include "Entity/Components/LineBatcherComponent.h"
#include "Entity/Components/VelocityComponent.h"
#include "Entity/Systems/DebugCameraEntitySystem.h"
#include "Entity/Components/StaticMeshComponent.h"
#include "Entity/Systems/UpdateTransformEntitySystem.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "Subsystems/FCameraManager.h"
#include "World/SceneRenderer.h"
#include "World/Entity/Components/RelationshipComponent.h"
#include "World/entity/systems/EntitySystem.h"

namespace Lumina
{
    CWorld::CWorld()
    {
    }

    void CWorld::Serialize(FArchive& Ar)
    {
        CObject::Serialize(Ar);
    
        if (Ar.IsWriting())
        {
            EntityRegistry.compact<>();
            auto View = EntityRegistry.view<entt::entity>(entt::exclude<SEditorComponent>);

            SIZE_T NumEntities = 0;
            TVector<entt::entity> Parents;
            Parents.reserve(View.size_hint());
            
            for (entt::entity entity : View)
            {
                // We only want to serialize top-level entities here, parents will serialize their children.
                if (EntityRegistry.all_of<SRelationshipComponent>(entity))
                {
                    auto& RelationshipComponent = EntityRegistry.get<SRelationshipComponent>(entity);
                    if (RelationshipComponent.Parent.IsValid())
                    {
                        continue;
                    }
                }
                
                Parents.emplace_back(entity);
                NumEntities++;
            }
            
            Ar << NumEntities;

            for (entt::entity Parent : Parents)
            {
                Entity TopLevelEntity(Parent, this);
                TopLevelEntity.Serialize(Ar);
            }
        }
        else if (Ar.IsReading())
        {
            EntityRegistry.clear<>();
            SIZE_T NumEntities = 0;
            Ar << NumEntities;

            for (SIZE_T i = 0; i < NumEntities; ++i)
            {
                Entity NewEntity(EntityRegistry.create(), this);
                NewEntity.Serialize(Ar);
            }
        }
    }

    void CWorld::InitializeWorld()
    {
        CameraManager = Memory::New<FCameraManager>();
        SceneRenderer = Memory::New<FSceneRenderer>(this);
        RegisterSystem(NewObject<CUpdateTransformEntitySystem>());
    }
    
    Entity CWorld::SetupEditorWorld()
    {
        RegisterSystem(NewObject<CDebugCameraEntitySystem>());

        Entity EditorEntity = ConstructEntity("Editor Entity");
        EditorEntity.AddComponent<SCameraComponent>();
        EditorEntity.AddComponent<SEditorComponent>();
        EditorEntity.AddComponent<SVelocityComponent>().Speed = 50.0f;
        EditorEntity.AddComponent<SHiddenComponent>();
        EditorEntity.GetComponent<STransformComponent>().SetLocation(glm::vec3(0.0f, 0.0f, 2.0f));

        SetActiveCamera(EditorEntity);

        return EditorEntity;
    }

    void CWorld::OnMarkedGarbage()
    {
        Memory::Delete(SceneRenderer);
        Memory::Delete(CameraManager);
        EntityRegistry.clear<>();
    }

    void CWorld::Tick(const FUpdateContext& Context)
    {
        if (Context.GetUpdateStage() == EUpdateStage::FrameStart)
        {
            DeltaTime = Context.GetDeltaTime();
            TimeSinceCreation += DeltaTime;
        }

        bool bWantsToRender = Context.GetUpdateStage() == EUpdateStage::FrameEnd;
        
        if ((IsPaused() && Context.GetUpdateStage() != EUpdateStage::Paused) && !bWantsToRender)
        {
            return;
        }
        
        for (CEntitySystem* System : SystemUpdateList[(uint32)Context.GetUpdateStage()])
        {
            System->Update(EntityRegistry, Context);
        }

        if (bWantsToRender)
        {
            SceneRenderer->Render(Context);
        }
    }

    void CWorld::ShutdownWorld()
    {
        Memory::Delete(SceneRenderer);
        Memory::Delete(CameraManager);
        
        GEngine->GetEngineSubsystem<FWorldManager>()->RemoveWorld(this);
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

    bool CWorld::RegisterSystem(CEntitySystem* NewSystem)
    {
        Assert(NewSystem != nullptr)
        
        NewSystem->World = this;
        NewSystem->Initialize();

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

    Entity CWorld::ConstructEntity(const FName& Name, const FTransform& Transform)
    {
        entt::entity NewEntity = EntityRegistry.create();
        EntityRegistry.emplace<SNameComponent>(NewEntity).Name = Name;
        EntityRegistry.emplace<STransformComponent>(NewEntity).Transform = Transform;
        
        return Entity(NewEntity, this);
    }
    
    void CWorld::CopyEntity(Entity& To, const Entity& From)
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

    void CWorld::ReparentEntity(Entity Child, Entity Parent)
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

    void CWorld::DestroyEntity(Entity Entity)
    {
        Assert(Entity.IsValid())
        EntityRegistry.destroy(Entity);
    }

    void CWorld::SetActiveCamera(Entity InEntity)
    {
        CameraManager->SetActiveCamera(InEntity);
    }

    SCameraComponent& CWorld::GetActiveCamera() const
    {
        return *CameraManager->GetCameraComponent();
    }

    const TVector<TObjectHandle<CEntitySystem>>& CWorld::GetSystemsForUpdateStage(EUpdateStage Stage)
    {
        return SystemUpdateList[uint32(Stage)];
    }
    
    void CWorld::DrawDebugLine(const glm::vec3& Start, const glm::vec3& End, const glm::vec4& Color, float Thickness, float Duration)
    {
        FLineBatcherComponent& Batcher = GetOrCreateLineBatcher();
        Batcher.DrawLine(Start, End, Color, Thickness, Duration);
    }

    void CWorld::DrawDebugBox(const glm::vec3& Center, const glm::vec3& Extents, const glm::quat& Rotation, const glm::vec4& Color, float Thickness, float Duration)
    {
        FLineBatcherComponent& Batcher = GetOrCreateLineBatcher();
        Batcher.DrawBox(Center, Extents, Rotation, Color, Thickness, Duration);
    }

    void CWorld::DrawDebugSphere(const glm::vec3& Center, float Radius, const glm::vec4& Color, uint8 Segments, float Thickness, float Duration)
    {
        FLineBatcherComponent& Batcher = GetOrCreateLineBatcher();
        Batcher.DrawSphere(Center, Radius, Color, Segments, Thickness, Duration);
    }

    void CWorld::DrawDebugCone(const glm::vec3& Apex, const glm::vec3& Direction, float AngleRadians, float Length, const glm::vec4& Color, uint8 Segments, uint8 Stacks, float Thickness, float Duration)
    {
        FLineBatcherComponent& Batcher = GetOrCreateLineBatcher();
        Batcher.DrawCone(Apex, Direction, AngleRadians, Length, Color, Segments, Stacks, Thickness, Duration);
    }

    void CWorld::DrawFrustum(const glm::mat4& Matrix, const glm::vec4& Color, float Thickness, float Duration)
    {
        FLineBatcherComponent& Batcher = GetOrCreateLineBatcher();
        Batcher.DrawFrustum(Matrix, Color, Thickness, Duration);
    }

    void CWorld::DrawArrow(const glm::vec3& Start, const glm::vec3& Direction, float Length, const glm::vec4& Color, float Thickness, float Duration, float HeadSize)
    {
        FLineBatcherComponent& Batcher = GetOrCreateLineBatcher();
        Batcher.DrawArrow(Start, Direction, Length, Color, Thickness, Duration, HeadSize);
    }
    
    FLineBatcherComponent& CWorld::GetOrCreateLineBatcher()
    {
        if (LineBatcherComponent)
        {
            return *LineBatcherComponent; 
        }
        
        Entity LineBatcherEntity = ConstructEntity("LineBatcher", FTransform());
        LineBatcherEntity.AddComponent<SHiddenComponent>();
        LineBatcherComponent = &LineBatcherEntity.AddComponent<FLineBatcherComponent>();
        
        return *LineBatcherComponent;
    }
}
