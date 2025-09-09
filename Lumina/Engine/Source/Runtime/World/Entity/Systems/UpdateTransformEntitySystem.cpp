#include "UpdateTransformEntitySystem.h"

#include "Core/Object/Class.h"
#include "TaskSystem/TaskSystem.h"
#include "World/Entity/Components/DirtyComponent.h"
#include "World/Entity/Components/EditorComponent.h"
#include "World/Entity/Components/RelationshipComponent.h"
#include "World/Entity/Components/RenderComponent.h"
#include "World/Entity/Components/Transformcomponent.h"

namespace Lumina
{
    
    void CUpdateTransformEntitySystem::Initialize()
    {
        
    }

    void CUpdateTransformEntitySystem::Shutdown()
    {
        
    }

    void CUpdateTransformEntitySystem::Update(FEntityRegistry& EntityRegistry, const FUpdateContext& UpdateContext)
    {
        LUMINA_PROFILE_SCOPE();
        
        auto Group = EntityRegistry.group<>(entt::get<STransformComponent>);
        auto RelationshipGroup = EntityRegistry.group<>(entt::get<STransformComponent, SRelationshipComponent>);
        auto CameraView = EntityRegistry.view<SCameraComponent>(entt::exclude<SEditorComponent>);

        Task::ParallelFor(Group.size(), [&](uint32 Index)
        {
            entt::entity entity = Group[Index];
            auto& transform = Group.get<STransformComponent>(entity);

            if (CameraView.contains(entity))
            {
                auto& Camera = CameraView.get<SCameraComponent>(entity);
                glm::vec3 UpdatedForward = transform.Transform.Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
                glm::vec3 UpdatedUp      = transform.Transform.Rotation * glm::vec3(0.0f, 1.0f,  0.0f);
    
                Camera.SetView(transform.Transform.Location, transform.Transform.Location + UpdatedForward, UpdatedUp);
            }
            
            if (RelationshipGroup.contains(entity))
            {
                auto& relationship = RelationshipGroup.get<SRelationshipComponent>(entity);
            
                if (relationship.Parent.IsValid())
                {
                    transform.WorldTransform = relationship.Parent.GetComponent<STransformComponent>().WorldTransform * transform.Transform;
                }
                else
                {
                    transform.WorldTransform = transform.Transform;
                }
            }
            else
            {
                transform.WorldTransform = transform.Transform;
            }
            
            transform.CachedMatrix = transform.WorldTransform.GetMatrix();
            
        });

        EntityRegistry.clear<FDirtyTransform>();
    }
}
