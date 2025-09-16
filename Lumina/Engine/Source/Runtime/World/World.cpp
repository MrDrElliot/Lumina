#include "World.h"

#include "WorldManager.h"
#include "Core/Engine/Engine.h"
#include "Core/Object/Class.h"
#include "Core/Object/ObjectIterator.h"
#include "Core/Object/Package/Package.h"
#include "Core/Serialization/MemoryArchiver.h"
#include "Core/Serialization/ObjectArchiver.h"
#include "EASTL/sort.h"
#include "Entity/Components/EditorComponent.h"
#include "Entity/Components/LineBatcherComponent.h"
#include "Entity/Components/VelocityComponent.h"
#include "Entity/Systems/EditorEntityMovementSystem.h"
#include "Entity/Systems/UpdateTransformEntitySystem.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "Subsystems/FCameraManager.h"
#include "TaskSystem/TaskSystem.h"
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
            GetEntityRegistry().compact<>();
            auto View = GetEntityRegistry().view<entt::entity>(entt::exclude<SEditorComponent>);

            SIZE_T NumEntities = 0;
            TVector<entt::entity> Parents;
            Parents.reserve(View.size_hint());
            
            for (entt::entity entity : View)
            {
                // We only want to serialize top-level entities here, parents will serialize their children.
                if (GetEntityRegistry().all_of<SRelationshipComponent>(entity))
                {
                    auto& RelationshipComponent = GetEntityRegistry().get<SRelationshipComponent>(entity);
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
            
            GetEntityRegistry().clear<>();
            SIZE_T NumEntities = 0;
            Ar << NumEntities;

            for (SIZE_T i = 0; i < NumEntities; ++i)
            {
                Entity NewEntity(GetEntityRegistry().create(), this);
                NewEntity.Serialize(Ar);
            }
        }
    }

    void CWorld::InitializeWorld()
    {
        LUM_ASSERT(CameraManager == nullptr)
        LUM_ASSERT(SceneRenderer == nullptr)
        
        CameraManager = Memory::New<FCameraManager>();
        SceneRenderer = Memory::New<FSceneRenderer>(this);

        TVector<TObjectHandle<CEntitySystem>> Systems;
        CEntitySystemRegistry::Get().GetRegisteredSystems(Systems);
        
        for (CEntitySystem* System : Systems)
        {
            if (System->GetRequiredUpdatePriorities())
            {
                CEntitySystem* DuplicateSystem = NewObject<CEntitySystem>(System->GetClass());
                RegisterSystem(DuplicateSystem);
            }
        }
    }
    
    Entity CWorld::SetupEditorWorld()
    {
        Entity EditorEntity = ConstructEntity("Editor Entity");
        EditorEntity.Emplace<SCameraComponent>();
        EditorEntity.Emplace<SEditorComponent>();
        EditorEntity.Emplace<SVelocityComponent>().Speed = 50.0f;
        EditorEntity.Emplace<SHiddenComponent>();
        EditorEntity.GetComponent<STransformComponent>().SetLocation(glm::vec3(0.0f, 0.0f, 2.0f));

        SetActiveCamera(EditorEntity);

        return EditorEntity;
    }

    void CWorld::Update(const FUpdateContext& Context)
    {
        LUMINA_PROFILE_SCOPE();

        const EUpdateStage Stage = Context.GetUpdateStage();
        
        if (Stage == EUpdateStage::FrameStart)
        {
            DeltaTime = Context.GetDeltaTime();
            TimeSinceCreation += DeltaTime;
        }
        
        FSystemContext SystemContext(this);
        
        auto& SystemVector = SystemUpdateList[(uint32)Stage];
        Task::ParallelFor((uint32)SystemVector.size(), [this, SystemVector, &SystemContext](uint32 Index)
        {

            CEntitySystem* System = SystemVector[Index];
            System->Update(SystemContext);
        });
    }

    void CWorld::Paused(const FUpdateContext& Context)
    {
        LUMINA_PROFILE_SCOPE();

        DeltaTime = Context.GetDeltaTime();
        TimeSinceCreation += DeltaTime;
        
        FSystemContext SystemContext(this);
        
        auto& SystemVector = SystemUpdateList[(uint32)EUpdateStage::Paused];
        Task::ParallelFor((uint32)SystemVector.size(), [this, SystemVector, &SystemContext](uint32 Index)
        {
            CEntitySystem* System = SystemVector[Index];
            System->Update(SystemContext);
        });
    }

    void CWorld::Render(FRenderGraph& RenderGraph)
    {
        LUMINA_PROFILE_SCOPE();

        SceneRenderer->RenderScene(RenderGraph);
    }

    void CWorld::ShutdownWorld()
    {
        Memory::Delete(SceneRenderer);
        Memory::Delete(CameraManager);
        
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
        entt::entity NewEntity = GetEntityRegistry().create();

        FString StringName(Name.c_str());
        StringName += "_" + eastl::to_string((int)NewEntity);
        
        GetEntityRegistry().emplace<SNameComponent>(NewEntity).Name = StringName;
        
        GetEntityRegistry().emplace<STransformComponent>(NewEntity).Transform = Transform;
        
        return Entity(NewEntity, this);
    }
    
    void CWorld::CopyEntity(Entity& To, const Entity& From)
    {
        entt::entity NewEntity = GetEntityRegistry().create();
        To = Entity(NewEntity, this);
        
        for (auto [id, storage]: GetEntityRegistry().storage())
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
                        for (SIZE_T j = i; j < ToRemove->Size - 1; ++j)
                        {
                            ToRemove->Children[j] = ToRemove->Children[j + 1];
                        }
    
                        --ToRemove->Size;
                        break;
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
        GetEntityRegistry().destroy(Entity);
    }

    void CWorld::SetActiveCamera(Entity InEntity)
    {
        CameraManager->SetActiveCamera(InEntity);
    }

    SCameraComponent& CWorld::GetActiveCamera() const
    {
        return *CameraManager->GetCameraComponent();
    }

    CWorld* CWorld::DuplicateWorldForPIE(CWorld* OwningWorld)
    {
        CPackage* OuterPackage = OwningWorld->GetPackage();
        if (OuterPackage == nullptr)
        {
            return nullptr;
        }

        TVector<uint8> Data;
        {
            FMemoryWriter Writer(Data);
            FObjectProxyArchiver Ar(Writer, true);

            OwningWorld->Serialize(Ar);
        }
        
        FMemoryReader Reader(Data);
        FObjectProxyArchiver Ar(Reader, true);
        
        CWorld* PIEWorld = NewObject<CWorld>();

        PIEWorld->Serialize(Ar);
        
        return PIEWorld;
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
        LineBatcherEntity.Emplace<SHiddenComponent>();
        LineBatcherComponent = &LineBatcherEntity.Emplace<FLineBatcherComponent>();
        
        return *LineBatcherComponent;
    }
}
